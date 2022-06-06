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


enum struct CommandType {
    RENDER_TEXT,
};

struct RenderCommand {
    enum CommandType type;
    int row;
    int col;
    size_t n;
};

#define AMOUNT_COMMANDS 1024

struct Renderer {

    
    float dx = 0.0f;
    float dy = 0.0f;
    int rows = 0;
    int columns = 0;
    char *text_buffer = NULL;
    size_t text_buffer_size = 0;
    // TODO: correct data structure
    RenderCommand commands[AMOUNT_COMMANDS];
    size_t num_commands = 0;

#ifdef __unix__
    Terminal t;
#else

    COLORREF _get_color(enum RenderColor color);
#endif

    void set_color(enum RenderColor color);
    void set_background_color(enum RenderColor color);
    void init(size_t rows, size_t columns);

    //void set_cursor_pos(int x, int y);
    //void reset_cursor();
    //void set_invert_color(bool value);
    //void set_cursor_visibility(bool value);
    void push_text(int row, int col, const char *text, size_t n);
    void clear_line();
    tuple<int, int> get_window_size();

    void clear_screen();
    void flush();
    void shutdown();
};
