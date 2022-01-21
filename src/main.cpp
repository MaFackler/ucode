#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

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

int main(int argc, char **argv) {
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

        if (c == 's') {
            e.row++;
        }
        if (c == 'w') {
            e.row--;
        }
        if (c == 'a') {
            e.col--;
        }
        if (c == 'd') {
            e.col++;
        }

        e.row = std::max(0, e.row);
        e.row = std::min(e.rows - 1, e.row);
        e.col = std::max(0, e.col);
        e.col = std::min(e.columns - 1, e.col);

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
