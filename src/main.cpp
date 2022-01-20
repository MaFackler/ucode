#include <iostream>
#include <string>

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

int main(int, char **) {
    t.init();
    std::atexit(exit_handler);
    auto size = t.get_window_size();
    e.columns = get<0>(size);
    e.rows = get<1>(size);
    //auto cursor_pos = t.get_cursor_pos();
    
    e.open_file("Makefile");
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

        t.reset_cursor();

        for (auto &line: e.lines) {
            t.clear_line();
            t.write(line.c_str());
            t.write("\r\n");
        }

        for (int i = e.lines.size() + 1; i < e.rows; ++i) {
            t.clear_line();
            t.write("~\r\n");
        }

        t.set_cursor_pos(e.col, e.row);

        t.flush();
    }

    return 0;
}
