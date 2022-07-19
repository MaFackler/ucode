//hey
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <functional>


#include "ucode_test.h"

#define MF_PLATFORM_USE_OPENGL
#define MF_PLATFORM_IMPLEMENTATION
#include "mf_platform.h"

#include "ucode_log.h"
#include "ucode_input.h"
#include "ucode_terminal.h"
#include "ucode_editor.h"
#include "ucode_command.h"
#include "ucode_renderer.h"

#include "../custom/config.h"


#include "ucode_log.cpp"
#include "ucode_input.cpp"
#include "ucode_file_chooser.cpp"
#include "ucode_editor.cpp"
#include "ucode_lexer.cpp"
#include "ucode_renderer.cpp"



using std::cout;
using std::get;
using std::string;
using std::map;


//static Terminal t;
static Editor e;



void exit_handler() {
    //t.reset_cursor();
    //t.clear_screen();
    //t.flush();
    //t.shutdown();
}


#if 0
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
            case '\t': def.key = Key::TAB; break;
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
    
    auto state = e.state();
    if (keybindings[state].find(def) != keybindings[state].end()) {
        ICommand* cmd = keybindings[state][def];
        cmd->key = def.key;
        cmd->execute(e);
    } else {
        // TODO: should this also be a command?
        char c = static_cast<char>(def.key);
        if (state == EditorState::BUFFER_INSERT && c >= ' ' && c <= '~') {
            e.insert_char((char) def.key);
        } else if (state == EditorState::OPEN_DIRECTORY && c >= ' ' && c <= '~') {
            e.chooser.filter_string += c;
            e.chooser.filter();
        }
    }
}
#endif

void draw_statusline() {
#if 0
    // Draw statusline
    t.set_invert_color(true);
    t.clear_line();
    string statusline;
    TerminalColor color = TerminalColor::DEFAULT;
    switch (e.state()) {
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
#endif
}

int main(int argc, char **argv) {

    LogInit();
    Debug << "Logging initialized";

    mfp_platform platform = {};
    mfp_init(&platform);
    mfp_window_open(&platform, "UCode", 0, 0, 1600, 900);

    UCODE_TEST;
    //t.init();
    std::atexit(exit_handler);
    //auto[screen_columns, screen_rows] = t.get_window_size();
    auto screen_columns = 30;
    auto screen_rows = 30;
    e.screen_columns = screen_columns;
    e.screen_rows = screen_rows - 1;  // TODO: -1 height of statusbar
    state_command_map keybindings;
    Keywords(e.lexer.keywords);
    Types(e.lexer.types);
    Init(e, keybindings);

    Renderer renderer;
    
    if (argc == 2) {
        e.open_file(argv[1]);
    }
    while (!e.quit && platform.window.isOpen) {

        //term_handle_key(t, keybindings, e);
        mfp_begin(&platform);
        renderer.clear();

        // begin
        //t.set_cursor_visibility(false);
        //t.reset_cursor();
        bool cursor_visible = true;
        auto state = e.state();

        if (state == EditorState::BUFFER_NORMAL || state == EditorState::BUFFER_INSERT) {
            size_t endline = e.lines.size();
            enum TokenType hl = TokenType::NONE;
            for (int row_index = e.scroll_offset; row_index < e.scroll_offset + e.screen_rows; ++row_index) {
                //t.clear_line();

                if (row_index < endline) {
                    auto &tokens = e.token_lines[row_index];
                    for (auto &token: tokens) {
                        if (token.type != hl) {
#if 0
                            auto c = TerminalColor::DEFAULT;
                            if (token.type == TokenType::NONE) {
                            } else if (token.type == TokenType::NUMBER) {
                                c = TerminalColor::MAGENTA;
                            } else if (token.type == TokenType::KEYWORD) {
                                c = TerminalColor::RED;
                            } else if (token.type == TokenType::TYPE) {
                                c = TerminalColor::YELLOW;
                            }
                            t.set_color(c);
                            hl = token.type;
#endif
                        }
                        if (token.chars.size()) {
                            //t.write(token.chars.c_str());
                        }
                    }
                    //t.write(e.lines[row_index].c_str());
                } else {
                    //t.write("~");
                }
                //t.write_new_line();
            }
        } else if (state == EditorState::OPEN_DIRECTORY) {

#if 0
            cursor_visible = false;
            t.clear_line();
            t.write(e.chooser.filter_string.c_str());
            t.write_new_line();

            size_t s = e.chooser.filtered.size();
            for (int i = 1; i < e.screen_rows; ++i) {
                int fetch_index = i - 1;
                if (fetch_index == 0) {
                    t.set_invert_color(true);
                }
                t.clear_line();
                if (fetch_index < s) {
                    int index = e.chooser.filtered[fetch_index];
                    t.write(e.chooser.files[index].c_str());
                }
                t.write_new_line();
                if (fetch_index == 0) {
                    t.set_invert_color(false);
                }
            }
#endif
        }

        draw_statusline();

#if 0
        t.set_cursor_pos(e.col, e.row - e.scroll_offset);
        t.set_cursor_visibility(cursor_visible);

        t.flush();
#endif
        mfp_end(&platform);
    }
    mfp_window_close(&platform);
    mfp_destroy(&platform);

    return 0;
}
