#include <cstdio>
#include <cwchar>
#include <cwctype>
#include "debuglib.hpp"
#include "line_sort.hpp"

FILE * out;

void print_line(const char *text, Line line) {
    fprintf(out, "%s", text + line.pos);
}

bool forward_comparator(const char *text, const Line &a, const Line &b) {
    if (a.pos == b.pos && a.len == b.len) return 0;
    int pos1 = 0, pos2 = 0;
    wchar_t c1 = 0, c2 = 0;
    while (c1 == c2 && pos1 < a.len && pos2 < b.len) {
        do {
            pos1 += mbrtowc(&c1, text + a.pos + pos1, a.len - pos1, NULL);
        } while (!iswalpha(c1) && pos1 < a.len);
        do {
            pos2 += mbrtowc(&c2, text + b.pos + pos2, b.len - pos2, NULL);
        } while (!iswalpha(c2) && pos2 < b.len);
    }

    if (pos1 >= a.len && pos2 >= b.len) {
        return 0;
    }
    if (pos1 >= a.len) {
        return 1;
    }
    if (pos2 >= b.len) {
        return 0;
    }
    return (c1 < c2); 
}

bool is_trailing_byte(char c) {
    const char trailing_mask = (char) (3 << 6);
    return ((c & trailing_mask) == ~((1 << 7) - 1));
}

bool backwards_comparator(const char *text, const Line &a, const Line &b) {
    int pos1 = a.len, pos2 = b.len;
    wchar_t c1 = 0, c2 = 0;
    while (c1 == c2 && pos1 > 0 && pos2 > 0) {
        do {
            do {
                pos1--;
            } while (is_trailing_byte(text[a.pos + pos1]));
            mbrtowc(&c1, text + a.pos + pos1, a.len - pos1, NULL);
        } while (!iswalpha(c1) && pos1 > 0);
        do {
            do {
                pos2--;
            } while (is_trailing_byte(text[b.pos + pos2]));
            mbrtowc(&c2, text + b.pos + pos2, b.len - pos2, NULL);
        } while (!iswalpha(c2) && pos1 > 0);
    } 

    if (pos1 == 0 && pos2 == 0) {
        return 0;
    }
    if (pos1 == 0) {
        return 1;
    }
    if (pos2 == 0) {
        return 0;
    }
    return (c1 < c2); 
}

void merge_sort_internal(const char *text, 
        std::vector<Line>::iterator buf_begin, std::vector<Line>::iterator buf_end, 
        const std::vector<Line>::iterator begin, const std::vector<Line>::iterator end,
        bool (*cmp) (const char *, const Line &, const Line &)) {
    int n = end - begin;
    if (n <= 1) return;
    if (n == 2) {
        if (!cmp(text, *begin, *(begin+1))) {
            std::swap(*begin, *(begin+1));
        }
        return;
    }
    int m = n/2;
    auto p_left = begin;
    auto p_right = begin + m;
    merge_sort_internal(text, buf_begin, buf_begin + m, p_left, p_right, cmp);
    merge_sort_internal(text, buf_begin + m, buf_end, p_right, end, cmp);
    auto p_buf = buf_begin;
    while (p_left != begin + m || p_right != end) {
        if (cmp(text, *p_left, *p_right)) {
            *p_buf = *p_left;
            p_buf++;
            p_left++;
        } else {
            *p_buf = *p_right;
            p_buf++;
            p_right++;
        }

        if (p_left == begin + m) {
            std::copy(p_right, end, p_buf);
            break;
        }

        if (p_right == end) {
            std::copy(p_left, begin + m, p_buf);
            break;
        }
    }
    std::copy(buf_begin, buf_end, begin);
}

void merge_sort(const char *text, std::vector<Line>::iterator begin, std::vector<Line>::iterator end, 
        bool (*cmp) (const char *, const Line &, const Line &)) {
    int n = end - begin;
    std::vector<Line> buf(n);
    merge_sort_internal(text, buf.begin(), buf.end(), begin, end, cmp);
}

std::vector<Line> find_lines(char *text, size_t size) {
    std::vector<Line> lines;
    Line current{0, 0};
    for (size_t i = 0; i < size; i++) {
        if (text[i] == '\n' || text[i] == '\r') {
            current.len = i - current.pos;
            lines.push_back(current);
            text[i] = 0;
            while (i+1 < size && (text[i+1] == '\n' || text[i+1] == '\r')) {
                i++;   
            }
            current.pos = i+1;
        } 
    }
    return lines;
}

void test_comparator(char *text, const Line &a, const Line &b, bool result, bool (*cmp) (const char *, const Line &, const Line &)) {
    DEBUG_RED(printf("Comparing lines starting at %d and %d", a.pos, b.pos));
    DEBUG_BLUE(print_line(text, a));
    DEBUG_BLUE(print_line(text, b));
    bool res = cmp(text, a, b);
    if (res) {
        printf("<  ");
    } else {
        printf(">= ");
    }
    if (res == result) {
        DEBUG_GREEN_BG(printf("[OK]"));
    } else {
        DEBUG_RED_BG(printf("[FAIL]"));
    }
    printf("\n");
}
