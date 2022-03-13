#pragma once
#include <assert.h>

#ifdef __unix__
#include "ucode_terminal.h"
#endif

using std::tuple;


enum struct RenderColor {
    BLACK = 0,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
};

struct Renderer {

    bool close = false;
#ifdef __unix__
    Terminal t;
#else
    HDC dc;
    RECT rect;
    HWND hwnd;
    MSG msg;
    std::vector<KeyDef> keys;
    SIZE size;
    size_t line_counter = 0;
    size_t column_counter = 0;

    COLORREF _get_color(enum RenderColor color);
#endif

    void init();

    void handle_input();
    void begin();
    void end();

    void set_color(enum RenderColor color);
    void set_background_color(enum RenderColor color);
    //void set_cursor_pos(int x, int y);
    //void reset_cursor();
    //void set_invert_color(bool value);
    //void set_cursor_visibility(bool value);
    void write(const char *text);
    void write_new_line();
    void clear_line();
    tuple<int, int> get_window_size();

    void clear_screen();
    void flush();
    void shutdown();
};
