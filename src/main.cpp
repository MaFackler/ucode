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
    auto[screen_columns, screen_rows] = t.get_window_size();
    e.screen_columns = screen_columns;
    e.screen_rows = screen_rows;
    
    if (argc == 2) {
        e.open_file(argv[1]);
    }
    while (1) {
        char c = t.read_char();
        if (c == ctrl_key('q'))
            break;

        int dx = 0; 
        int dy = 0;
        if (c == TERM_ESCAPE_CODE) {
            char c1 = t.read_char();
            if (c1 == '[') {
                char c2 = t.read_char();
                switch (c2) {
                    case 'A':
                        // UP
                        dy--;
                        break;
                    case 'B':
                        // DOWN
                        dy++;
                        break;
                    case 'C':
                        // RIGHT
                        dx++;
                        break;
                    case 'D':
                        // LEFT
                        dx--;
                        break;
                    default:
                        break;
                }
            }
        }

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

        if (c == ctrl_key('s')) {
            e.save_file();
        }

        if (c == 0x1B) {
            // Escape
        }

        if (c == 0x0D) {
            // CR
            e.insert_new_line();
        }

        e.move_cursor(dx, dy);

        // begin
        t.set_cursor_visibility(false);
        t.reset_cursor();


        int endline = e.lines.size();
        for (int row_index = 0; row_index < e.screen_rows - 1; ++row_index) {
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
        statusline.append(e.screen_columns - statusline.size(), ' ');
        t.write(statusline.c_str());
        t.set_invert_color(false);

        t.set_cursor_pos(e.col, e.row);
        t.set_cursor_visibility(true);

        t.flush();
    }

    return 0;
}
