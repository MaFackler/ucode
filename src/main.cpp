#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <functional>

//#define DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

#include "ucode_log.h"
#include "ucode_input.h"
#include "ucode_terminal.h"
#include "ucode_editor.h"
#include "ucode_command.h"

#include "../custom/config.h"



using std::cout;
using std::get;
using std::string;
using std::map;


static Terminal t;
static Editor e;



void exit_handler() {
    t.reset_cursor();
    t.clear_screen();
    t.flush();
    t.shutdown();
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

void term_handle_key(Terminal &t, state_command_map &keybindings, Editor &e) {
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

    if (c && def.key == Key::NONE) {
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
        def.ctrl.value = true;
        def.key = Key(0x60 | static_cast<int>(def.key));
    }
    
    if (keybindings[e.state].find(def) != keybindings[e.state].end()) {
        ICommand* cmd = keybindings[e.state][def];
        cmd->key = def.key;
        cmd->execute(e);
    } else {
        // TODO: should this also be a command?
        if (e.state == EditorState::BUFFER_INSERT && std::isalnum((char) def.key)) {
            e.insert_char((char) def.key);
        }
    }
}

void draw_statusline() {
    // Draw statusline
    t.set_invert_color(true);
    t.clear_line();
    string statusline;
    TerminalColor color = TerminalColor::DEFAULT;
    switch (e.state) {
        case EditorState::BUFFER_NORMAL:
            statusline += "NORMAL";
            break;
        case EditorState::BUFFER_INSERT:
            statusline += "INSERT";
            color = TerminalColor::YELLOW;
            break;
        case EditorState::OPEN_DIRECTORY:
            statusline += "OPEN";
            break;
    }
    statusline += " " + e.get_current_filename();
    statusline.append(e.screen_columns - statusline.size(), ' ');
    t.set_color(color);
    t.write(statusline.c_str());
    t.set_invert_color(false);
    t.set_color(TerminalColor::DEFAULT);
}

int main(int argc, char **argv) {

    LogInit();
    Debug << "Logging initialized";
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


    state_command_map keybindings;
    Init(keybindings);
    
    if (argc == 2) {
        e.open_file(argv[1]);
    }
    while (!e.quit) {

        term_handle_key(t, keybindings, e);


        // begin
        t.set_cursor_visibility(false);
        t.reset_cursor();
        bool cursor_visible = true;


        auto draw_line = +[](int row_index) { (void) row_index; };

        if (e.state == EditorState::BUFFER_NORMAL || e.state == EditorState::BUFFER_INSERT) {
            draw_line = +[](int row_index) {
                int endline = e.lines.size();
                if (row_index < endline) {
                    t.write(e.lines[row_index].c_str());
                } else {
                    t.write("~");
                }
            };
        } else if (e.state == EditorState::OPEN_DIRECTORY) {
            draw_line = +[](int row_index) {
                int endline = e.files.size();
                if (row_index < endline) {
                    if (row_index ==  e.files.index())
                        t.set_invert_color(true);

                    t.write(e.files[row_index].c_str());

                    if (row_index ==  e.files.index())
                        t.set_invert_color(false);
                }
            };
            cursor_visible = false;
        }

        for (int row_index = 0; row_index < e.screen_rows - 1; ++row_index) {
            t.clear_line();
            draw_line(row_index);
            t.write_new_line();
        }
        draw_statusline();

        t.set_cursor_pos(e.col, e.row);
        t.set_cursor_visibility(cursor_visible);

        t.flush();
    }

    return 0;
}
