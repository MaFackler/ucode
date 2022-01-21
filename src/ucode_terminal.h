#pragma once
#include <iostream>
#include <tuple>
#include <string>
#include <sstream>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

using std::cout;
using std::tuple;
using std::string;

#define TERM_ESCAPE(s) "\x1b[" s

struct Terminal {

    void init();
    void shutdown();

    char read_char();
    void clear_screen();
    void clear_line();
    void reset_cursor();
    void set_invert_color(bool value);
    void set_cursor_visibility(bool visible);
    void set_cursor_pos(int col, int row);
    void write(const char *s);
    void write_escape(const char *s);

    void flush();

    tuple<int, int> get_window_size();
    tuple<int, int> get_cursor_pos();

private:
    string buffer;
    struct termios orginal;
};

