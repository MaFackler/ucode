#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

//#define DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

#include "ucode_terminal.h"
#include "ucode_editor.h"

#define CTRL_KEY(k) ((k) & 0x1f)

using std::cout;
using std::get;
using std::string;


static Terminal t;
static Editor e;

void exit_handler() {
    t.reset_cursor();
    t.clear_screen();
    t.flush();
    t.shutdown();
}

constexpr char ctrl_key(char k) {
    return k & 0x1f;
}

#ifndef DOCTEST_CONFIG_DISABLE
int handle_doctest(int argc, char **argv) {
    doctest::Context ctx;
    //ctx.setOption("--no-run", true);
    ctx.applyCommandLine(argc, argv);
    int res = ctx.run();
    if (ctx.shouldExit())
        return 1;
    
    return res;
}
#endif

int main(int argc, char **argv) {

#ifndef DOCTEST_CONFIG_DISABLE
    int should_close = handle_doctest(argc, argv);
    if (should_close) {
        return should_close;
    }
#endif
    t.init();
    std::atexit(exit_handler);
    auto size = t.get_window_size();
    e.columns = get<0>(size);
    e.rows = get<1>(size);
    //auto cursor_pos = t.get_cursor_pos();
    
    if (argc == 2) {
        e.open_file(argv[1]);
    }
    while (1) {
        char c = t.read_char();
        if (c == ctrl_key('q'))
            break;

        int dx = 0; 
        int dy = 0;
        if (c == ctrl_key('n')) {
            dy++;
        }
        if (c == ctrl_key('p')) {
            dy--;
        }
        if (c == ctrl_key('h')) {
            dx--;
        }
        if (c == ctrl_key('l')) {
            dx++;
        }

        if (std::isalnum(c)) {
            e.insert_char(c);
        }

        e.move_cursor(dx, dy);

        // begin
        t.set_cursor_visibility(false);
        t.reset_cursor();


        int endline = e.lines.size();
        for (int row_index = 0; row_index < e.rows - 1; ++row_index) {
            t.clear_line();
            if (row_index < endline) {
                t.write(e.lines[row_index].c_str());
            } else {
                t.write("~");
            }
            t.write("\r\n");
        }

        // Draw statusline
        t.set_invert_color(true);
        t.clear_line();
        string statusline = e.buffer_name;
        statusline.append(e.columns - statusline.size(), ' ');
        t.write(statusline.c_str());
        t.set_invert_color(false);

        t.set_cursor_pos(e.col, e.row);
        t.set_cursor_visibility(true);

        t.flush();
    }

    return 0;
}
