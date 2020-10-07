#pragma once
//-----------------------DEBUG MACROSES---------------------
#ifdef COLORS

#define DEBUG_COLOR(expr, color) \
    fprintf(out, "\x1b[" #color "m");\
    (expr);\
    fprintf(out, "\x1b[0m \n");

#else

#define DEBUG_COLOR(expr, color) \
    (expr);\
    fprintf(out, "\n");

#endif

#define DEBUG_RED(expr) \
    DEBUG_COLOR(expr, 31);

#define DEBUG_BLUE(expr) \
    DEBUG_COLOR(expr, 34)

#define DEBUG_GREEN_BG(expr) \
    DEBUG_COLOR(expr, 42);

#define DEBUG_YELLOW_BG(expr) \
    DEBUG_COLOR(expr, 43)

#define DEBUG_RED_BG(expr) \
    DEBUG_COLOR(expr, 41)
//---------------------END DEBUG MACROSES-------------------

