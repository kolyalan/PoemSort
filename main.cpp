#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <cwchar>
#include <cstring>
#include "debuglib.hpp"
#include "line_sort.hpp"

extern FILE *out;

void help() {
    std::cerr << "The program should be called as: ./main input_file output_file" << std::endl;
}

bool get_filenames(int argc, char * argv[], char **filename, char **sorted_file, 
            char ** back_sorted_file, char **unsorted_file);

bool checked_freopen(char *filename);
char *open_and_mmap(char *filename, size_t *size);
void test_functions(char *text, std::vector<Line> lines);
void print_all_lines(char *text, std::vector<Line> lines);


int main(int argc, char *argv[]) {
    std::setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));
    char * filename         = NULL;
    char * sorted_file      = NULL;
    char * back_sorted_file = NULL;
    char * unsorted_file    = NULL;

    if (get_filenames(argc, argv, &filename, &sorted_file, &back_sorted_file, &unsorted_file) != 0) {
        return 1;
    }

    size_t size;
    char * text = open_and_mmap(filename, &size);
    
    std::vector<Line> lines = find_lines(text, size);

#ifdef TEST
    test_functions(text, lines);
#endif
    
    if (checked_freopen(sorted_file) != 0) {
        return 1;
    }
    merge_sort(text, lines.begin(), lines.end(), forward_comparator);
    DEBUG_YELLOW_BG(fprintf(out, "Sorted lines:"));
    print_all_lines(text, lines);
    fprintf(out, "\n");
    
    if(checked_freopen(back_sorted_file) != 0) {
        return 1;
    }
    merge_sort(text, lines.begin(), lines.end(), backwards_comparator);
    DEBUG_YELLOW_BG(fprintf(out, "Reverse sorted lines:"));
    print_all_lines(text, lines);
    fprintf(out, "\n");

    if (checked_freopen(unsorted_file) != 0) {
        return 1;
    }
    DEBUG_YELLOW_BG(fprintf(out, "Initial order:"));
    size_t p = 0;
    while (p < size) {
        p += fprintf(out, "%s\n", text + p);
    }
    
    munmap(text, size);
    fclose(out);
    return 0;
}

bool get_filenames(int argc, char * argv[], char **filename, char **sorted_file, 
            char ** back_sorted_file, char **unsorted_file) {
#ifndef TEST
    if (argc < 2) {
        std::cerr << "Input file not specified" << std::endl;
        help();
        return 1;
    }
    *filename = argv[1];
    if (argc < 3) {
        std::cerr << "Sorted output file not specified" << std::endl;
        help();
        return 1;
    }
    *sorted_file = argv[2];
    if (argc < 4) {
        std::cerr << "Backwards sorted output file not specified" << std::endl;
        help();
        return 1;
    }
    *back_sorted_file = argv[3];
    if (argc < 5) {
        std::cerr << "Unsorted output file not specified" << std::endl;
        help();
        return 1;
    }
    *unsorted_file = argv[4];
#else
    *filename = (char *)calloc(20, sizeof(**filename));
    memcpy(*filename, "onegin_small.txt", 17) ;
    out = stdout;
#endif
    return 0;
}

bool checked_freopen(char *filename) {
#ifndef TEST
    out = freopen(filename, "w", out);
    if (out == NULL) {
        std::cerr << "Unable to open file \"" << filename << "\"" << std::endl;
        return 1;
    }
#endif
    return 0;
}

char *open_and_mmap(char *filename, size_t *size) {
    int fd;
    struct stat file;
    if (stat(filename, &file) != 0 || (fd = open(filename, O_RDONLY)) == -1) {
        std::cerr << "Could not open the input file" << std::endl;
        return NULL;
    }
    *size = file.st_size;
    
    char *text = (char *) mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd,  0);
    if (text == MAP_FAILED) {
        std::cerr << "Could not load file in memory" << std::endl;
        return NULL; 
    }
    close(fd);
    return text;
}

void test_functions(char *text, std::vector<Line> lines) {
    DEBUG_YELLOW_BG(fprintf(out, "Testing line detection."));
    for (size_t i = 0; i < lines.size(); i++) {
        if (i%2) {
            DEBUG_RED(print_line(text, lines[i]));
        } else {
            DEBUG_BLUE(print_line(text, lines[i]));
        }
    }

    DEBUG_YELLOW_BG(fprintf(out, "Testing forward_comparator"));
    const int test_num = 10;
    int line_a[test_num]           = { 1,  1,  5,  5,  7, 10, 11, 23, 28, 32};
    int line_b[test_num]           = {10,  3, 19, 37,  4,  1, 17, 13,  3, 32};
    bool forward_result[test_num]  = { 1,  1,  0,  0,  1,  0,  0,  0,  0,  0};
    bool backward_result[test_num] = { 0,  0,  0,  0,  0,  1,  0,  0,  1,  0};
    for (int i = 0; i < test_num; i++) {
        test_comparator(text, lines[line_a[i]], lines[line_b[i]], forward_result[i], forward_comparator);
    }
    DEBUG_YELLOW_BG(fprintf(out, "Testing backwards comparator"));
    for (int i = 0; i < test_num; i++) {
        test_comparator(text, lines[line_a[i]], lines[line_b[i]], backward_result[i], backwards_comparator);
    }
    fprintf(out, "\n");
}

void print_all_lines(char *text, std::vector<Line> lines) {
    for (size_t i = 0; i < lines.size(); i++) {
        if (i%2) {
            DEBUG_RED(print_line(text, lines[i]));
        } else {
            DEBUG_BLUE(print_line(text, lines[i]));
        }
    }
}
