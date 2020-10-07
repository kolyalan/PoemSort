#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <cwchar>
#include "debuglib.hpp"
#include "line_sort.hpp"

extern FILE *out;

void help() {
    std::cerr << "The program should be called as: ./main input_file output_file" << std::endl;
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    srand(time(NULL));
    struct stat file;
    int fd;
#ifndef TEST
    if (argc < 2) {
        std::cerr << "Input file not specified" << std::endl;
        help();
        return 1;
    }
    char * filename = argv[1];
    if (argc < 3) {
        std::cerr << "Sorted output file not specified" << std::endl;
        help();
        return 1;
    }
    char * sorted_file = argv[2];
    if (argc < 4) {
        std::cerr << "Backwards sorted output file not specified" << std::endl;
        help();
        return 1;
    }
    char * back_sorted_file = argv[3];
    if (argc < 5) {
        std::cerr << "Unsorted output file not specified" << std::endl;
        help();
        return 1;
    }
    char * unsorted_file = argv[4];
#else
    char filename[] = "onegin_small.txt";
    out = stdout;
#endif
    if (stat(filename, &file) != 0 || (fd = open(filename, O_RDONLY)) == -1) {
        std::cerr << "Could not open the file" << std::endl;
        return 1;
    }
    size_t size = file.st_size;
    
    char *text = (char *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd,  0);
    if (text == MAP_FAILED) {
        std::cerr << "Could not load file in memory" << std::endl;
    }
    close(fd);
    
    std::vector<Line> lines = find_lines(text, size);

#ifdef TEST
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
#endif
    
#ifndef TEST
    out = fopen(sorted_file, "w");
    if (out == NULL) {
        std::cerr << "Unable to open sorted output file" << std::endl;
        return 1;
    }
#endif
    merge_sort(text, lines.begin(), lines.end(), forward_comparator);
    DEBUG_YELLOW_BG(fprintf(out, "Sorted lines:"));
    for (size_t i = 0; i < lines.size(); i++) {
        if (i%2) {
            DEBUG_RED(print_line(text, lines[i]));
        } else {
            DEBUG_BLUE(print_line(text, lines[i]));
        }
    }

#ifndef TEST
    out = freopen(back_sorted_file, "w", out);
    if (out == NULL) {
        std::cerr << "Unable to open backwards sorted output file" << std::endl;
        return 1;
    }
#endif
    
    merge_sort(text, lines.begin(), lines.end(), backwards_comparator);
    fprintf(out, "\n");
    DEBUG_YELLOW_BG(fprintf(out, "Reverse sorted lines:"));
    for (size_t i = 0; i < lines.size(); i++) {
        if (i%2) {
            DEBUG_RED(print_line(text, lines[i]));
        } else {
            DEBUG_BLUE(print_line(text, lines[i]));
        }
    }

#ifndef TEST
    out = freopen(unsorted_file, "w", out);
    if (out == NULL) {
        std::cerr << "Unable to open unsorted output file" << std::endl;
        return 1;
    }
#endif

    fprintf(out, "\n");
    DEBUG_YELLOW_BG(fprintf(out, "Initial order:"));
    size_t p = 0;
    while (p < size) {
        p += fprintf(out, "%s\n", text + p);
    }
    
    munmap(text, size);
    fclose(out);
    return 0;
}

