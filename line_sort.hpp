#pragma once
#include <vector>
#include <functional>
#include <cstdio>

struct Line {
    int pos;
    int len;
};

///----------------------------------------------------------------------
/// Prints a line from text starting from line.pos ending at '\0' in the string.
/// 
/// @param text a big piece of text, in which line.pos will be looked for.
/// @param line a struct describing which line to look for.
///
///----------------------------------------------------------------------
void print_line(const char *text, Line line);

///----------------------------------------------------------------------
/// Compares lines a and b from text. Returns true if line a is 
///     lexicorgaphically LESS than b. Ignores all non-letter symbols in 
///     system locale.
///
/// @param text a big piece of text, other parameters refer to it.
/// @param a    description of the first line to compare.
/// @param b    description of the second line to compare.
///
///----------------------------------------------------------------------
bool forward_comparator(const char *text, const Line &a, const Line &b);

///----------------------------------------------------------------------
/// Simply checks whether the value of c can be the first byte in UTF-8
///     symbol. Returns 1 if not.
///
/// @param c    the byte to check
///
///----------------------------------------------------------------------
bool is_trailing_byte(char c);

///----------------------------------------------------------------------
/// Compares reversed lines a and b from text. Returns true if reversed 
///     line a is lexicorgaphically LESS than reversed b. So, after 
///     sorting with this comparator rhyming lines should stay close. 
///     Ignores all non-letter symbols in system locale.
///
/// @param text a big piece of text, other parameters refer to it.
/// @param  a   description of the first line to compare.
/// @param  b   description of the second line to compare.
///
///----------------------------------------------------------------------
bool backwards_comparator(const char *text, const Line &a, const Line &b);

///----------------------------------------------------------------------
/// Just a regular merge sort algorithm. Sorts Line objects in [begin, end)
///     interval. Uses [buf_begin, buf_end) as a temporary buffer, 
///     supports only vector containers. Line objects must refer to text.
///
/// @param text         a big piece of text, other parameters refer to it.
/// @param buf_begin    iterator to the beginning of temporary vector
/// @param buf_end      iterator to the end of temporary vector
/// @param begin        iterator to the beginning of vector being sorted.
/// @param end          iterator to the end of vector being sorted.
/// @param cmp          comparator function. Must return 1 if the second argument is LESS than the third.
///
///----------------------------------------------------------------------
void merge_sort_internal(const char *text, 
        std::vector<Line>::iterator buf_begin, std::vector<Line>::iterator buf_end, 
        const std::vector<Line>::iterator begin, const std::vector<Line>::iterator end,
        std::function<bool(const char*, const Line &, const Line &)> cmp);

///----------------------------------------------------------------------
/// Just a regular merge sort algorithm. Sorts Line objects in [begin, end)
///     interval. Supports only vector containers. 
///     Line objects must refer to text. Actually, it allocates internal
//      buffer and calls recursive merge_sort_internal().
///
/// @param text     a big piece of text, other parameters refer to it.
/// @param begin    iterator to the beginning of vector being sorted.
/// @param end      iterator to the end of vector being sorted.
/// @param cmp      comparator function. Must return 1 if the second argument is LESS than the third.
///
///----------------------------------------------------------------------
void merge_sort(const char *text, std::vector<Line>::iterator begin, std::vector<Line>::iterator end, 
        std::function<bool(const char*, const Line &, const Line &)> cmp);

///----------------------------------------------------------------------
/// Simple testing function for checking comparators.
///
/// @param text     a big piece of text, other parameters refer to it.
/// @param a        the first line to compare
/// @param b        the second line to compare
/// @param result   expected result of comparison. 1 for LESS. 0 for MORE OR EQUAL.
/// @param cmp      comparator function to test.
///
///----------------------------------------------------------------------
void test_comparator(char *text, const Line &a, const Line &b, bool result, bool (*cmp) (const char *, const Line &, const Line &));

///----------------------------------------------------------------------
/// Looking for lines in text, lists them in std::vector<Line> as return value
///     also replaces '\n' at the end of detected lines to '\0' so the lines
///     become easier to manipulate.
///
/// @param text the text to look for lines in.
/// @param size size of the text in bytes.
///
///----------------------------------------------------------------------
std::vector<Line> find_lines(char *text, size_t size);



