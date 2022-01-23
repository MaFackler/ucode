#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <functional>

//#define DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

#include "ucode_input.h"
#include "ucode_terminal.h"
#include "ucode_editor.h"
#include "ucode_command.h"

#define CTRL_KEY(k) ((k) & 0x1f)

using std::cout;
using std::get;
using std::string;
using std::map;


static Terminal t;
static Editor e;

struct KeyDefHash {
    size_t operator()(const KeyDef &p) const {
        auto h1 = std::hash<int>()(static_cast<int>(p.key));
        auto h2 = std::hash<bool>()(p.ctrl);
        return h1 ^ h2;
    }
};

typedef std::unordered_map<KeyDef, ICommand*, KeyDefHash> command_map;

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

void term_handle_key(Terminal &t, command_map &keybindings, Editor &e) {
    char c = t.read_char();

    KeyDef def;

    // Handle escape codes
    if (c == TERM_ESCAPE_CODE) {
        char c1 = t.read_char();
        if (c1 == '[') {
            char c2 = t.read_char();
            switch (c2) {
                case 'A':
                    def.key = Key::UP;
                    break;
                case 'B':
                    def.key = Key::DOWN;
                    break;
                case 'C':
                    def.key = Key::RIGHT;
                    break;
                case 'D':
                    // LEFT
                    def.key = Key::LEFT;
                    break;
                default:
                    break;
            }
        }
    }



    if (def.key == Key::NONE) {
        switch (c) {
            case 0x0D: def.key = Key::RETURN; break;
            case 0x1B: def.key = Key::ESCAPE; break;
            default: def.key = (Key) c;
        }
    }

    if (def.key != Key::NONE && def.key < Key::SPACE) {
        // VT 100 ctrl handling
        // NOTE: Assume all key lower than space are ctrl keys
        // except Ascii 0
        def.ctrl = true;
        def.key = Key(0x60 | static_cast<int>(def.key));
    }
    if (keybindings.find(def) != keybindings.end()) {
        ICommand* cmd = keybindings[def];
        cmd->key = def.key;
        cmd->execute(e);
    } else {
        // TODO: should this also be a command?
        if (std::isalnum((char) def.key)) {
            e.insert_char((char) def.key);
        } else if (def.key == Key::RETURN) {
            e.insert_new_line();
        }

    }
    if (c == 'q') {
        cout << "got\n";
    }

    if (c == ctrl_key('h')) {
        char ctrl_cmp = ctrl_key('q');
        char ctrl_foo = ctrl_key('a');
        cout << "got\n" << ctrl_cmp << ctrl_foo;
    }
}

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


    CmdMoveCursor move_col_right(1, 0);
    CmdMoveCursor move_col_left(-1, 0);
    CmdMoveCursor move_row_up(0, -1);
    CmdMoveCursor move_row_down(0, 1);
    CmdQuit quit;
    command_map keybindings;
    keybindings[Key::LEFT] = &move_col_left;
    keybindings[Key::RIGHT] = &move_col_right;
    keybindings[Key::DOWN] = &move_row_down;
    keybindings[Key::UP] = &move_row_up;
    keybindings[(Key) 'z'] = &quit;
    keybindings[KeyDef('q', true)] = &quit;
    
    if (argc == 2) {
        e.open_file(argv[1]);
    }
    while (!e.quit) {

        term_handle_key(t, keybindings, e);

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
