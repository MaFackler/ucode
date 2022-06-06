#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <functional>

#include "ucode_test.h"
#include "ucode_input.h"
#include "ucode_platform.h"
#include "ucode_log.h"
#include "ucode_renderer.h"
#include "ucode_editor.h"
#include "ucode_command.h"

#include "../custom/config.h"

#include "ucode_input.cpp"
#include "ucode_platform.cpp"
#include "ucode_renderer.cpp"
#include "ucode_editor.cpp"
#include "ucode_lexer.cpp"



using std::cout;
using std::get;
using std::string;
using std::map;


static Renderer r;
static Editor e;



void exit_handler() {
    // r.reset_cursor();
    // r.clear_screen();
    // r.flush();
    // r.shutdown();
}


void handle_key(state_command_map &keybindings, Editor &e, KeyDef &def) {
    //KeyDef def;
    //if (c && def.key == Key::NONE) {
    //    switch (c) {
    //        case 0x0D: def.key = Key::RETURN; break;
    //        case 0x1B: def.key = Key::ESCAPE; break;
    //        default: def.key = (Key) c;
    //    }
    //}

    if (def.key != Key::NONE && def.key < Key::SPACE) {
        // VT 100 ctrl handling
        // NOTE: Assume all key lower than space are ctrl keys
        // except Ascii 0
        def.ctrl.value = true;
        def.key = Key(0x60 | static_cast<int>(def.key));
    }
    
    printf("key to parse %c\n", def.key);
    auto state = e.state();
    if (keybindings[state].find(def) != keybindings[state].end()) {
        printf("got one\n");
        ICommand* cmd = keybindings[state][def];
        cmd->key = def.key;
        cmd->execute(e);
    } else {
        // TODO: should this also be a command?
        char c = static_cast<char>(def.key);
        if (state == EditorState::BUFFER_INSERT && c >= ' ' && c <= '~') {
            e.insert_char((char) def.key);
        }
    }
}

#ifdef __unix__
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
    
    auto state = e.state();
    print("key %c\n", c);
    if (keybindings[state].find(def) != keybindings[state].end()) {
        ICommand* cmd = keybindings[state][def];
        cmd->key = def.key;
        cmd->execute(e);
    } else {
        // TODO: should this also be a command?
        char c = static_cast<char>(def.key);
        if (state == EditorState::BUFFER_INSERT && c >= ' ' && c <= '~') {
            e.insert_char((char) def.key);
        }
    }
}
#endif

void draw_statusline() {
    // Draw statusline
    // r.set_invert_color(true);
    // r.clear_line();
    string statusline;
#if 0
#TODO color
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
    t.set_color(color);
#endif
    statusline += " " + e.get_current_filename();
    statusline.append(e.screen_columns - statusline.size(), ' ');
    // r.write(statusline.c_str());
    // r.set_invert_color(false);
    // r.set_color(TerminalColor::DEFAULT);
}

int main(int argc, char **argv) {

    LogInit();
    Debug << "Logging initialized";

    UCODE_TEST;
    Platform p;
    p.open_window(800, 600);
    r.dx = p.size.cx;
    r.dy = p.size.cy;
    r.init(100, 100);
    std::atexit(exit_handler);
    // auto[screen_columns, screen_rows] = r.get_window_size();
    // e.screen_columns = screen_columns;
    // e.screen_rows = screen_rows - 1;  // TODO: -1 height of statusbar
    e.screen_rows = 40;
    e.screen_columns = 20;
    state_command_map keybindings;
    Keywords(e.lexer.keywords);
    Types(e.lexer.types);
    Init(keybindings);
    
    if (argc == 2) {
        e.open_file(argv[1]);
    }

    while (!e.quit && !p.close) {

        //term_handle_key(t, keybindings, e);
        p.begin();

        for (KeyDef &def: p.keys) {
            handle_key(keybindings, e, def);
        }

        // begin
        bool cursor_visible = true;
        auto state = e.state();

        if (state == EditorState::BUFFER_NORMAL || state == EditorState::BUFFER_INSERT) {
            int endline = e.lines.size();
            enum TokenType hl = TokenType::NONE;
            for (int row_index = e.scroll_offset; row_index < e.scroll_offset + e.screen_rows; ++row_index) {

                if (row_index < endline) {
                    auto &tokens = e.token_lines[row_index];
                    int col = 0;
                    for (auto &token: tokens) {
                        if (token.type != hl) {
                            auto c = RenderColor::BLACK;
                            if (token.type == TokenType::NONE) {
                            } else if (token.type == TokenType::NUMBER) {
                                c = RenderColor::RED;
                            } else if (token.type == TokenType::KEYWORD) {
                                c = RenderColor::GREEN;
                            } else if (token.type == TokenType::TYPE) {
                                c = RenderColor::BLUE;
                            }
                            r.set_color(c);
                            hl = token.type;
                        }
                        if (token.chars.size()) {
                            //r.write(token.chars.c_str());
                            int s = token.chars.size();
                            //printf("draw on row %d col %d string '%s' %d\n", row_index, col, token.chars.c_str(), s);
                            r.push_text(row_index, col, token.chars.c_str(), s);
                            col += s;
                        }
                    }
                    //t.write(e.lines[row_index].c_str());
                } else {
                }
            }
        } else if (state == EditorState::OPEN_DIRECTORY) {

            int endline = e.files.size();
            for (int i = 0; i < e.screen_rows; ++i) {
                r.clear_line();
                if (i < endline) {
                    if (i ==  e.files.index()) {
                        r.set_background_color(RenderColor::BLUE);
                    }
                        

                    auto &file = e.files[i];
                    if (std::filesystem::is_directory(file)) {
                        r.set_color(RenderColor::RED);
                    } else {
                        r.set_color(RenderColor::GREEN);
                    }

                    //r.write(file.filename().string().c_str());
                    auto &s = file.filename().string();
                    r.push_text(i, 0, s.c_str(), s.size());

                    if (i ==  e.files.index()) {
                        r.set_background_color(RenderColor::WHITE);
                    }

                }
            }
            cursor_visible = false;
        }

        //draw_statusline();

        //r.set_cursor_pos(e.col, e.row - e.scroll_offset);
        //r.set_cursor_visibility(cursor_visible);

        r.flush();

        for (size_t i = 0; i < r.num_commands; ++i) {
            RenderCommand &cmd = r.commands[i];
            if (cmd.type == CommandType::RENDER_TEXT) {
                char *text = &r.text_buffer[cmd.row * r.columns + cmd.col]; 
                p.draw_text(cmd.col * r.dx, cmd.row * r.dy, text, cmd.n);
            }
        }
        r.num_commands = 0;
        p.end();

        RECT viewport;
        GetClientRect(p.hwnd, &viewport);

        if (p.frame_buffer.width && p.frame_buffer.height) {
            HDC dc = GetDC(p.hwnd);

            BITMAPINFO bi = {0};
            BITMAPINFOHEADER *header = &bi.bmiHeader;
            header->biSize = sizeof(*header);
            header->biWidth = p.frame_buffer.width;
            header->biHeight = p.frame_buffer.height;
            header->biPlanes = 1;
            header->biBitCount = 32;
            header->biCompression = BI_RGB;

            rgba8 value = {0xFF, 0x00, 0x00, 0xFF};

            for (size_t i = 0; i < p.frame_buffer.width * p.frame_buffer.height; ++i) {
                p.frame_buffer.data[i] = value;
            }

            StretchDIBits(dc,
                          0, 0,
                          viewport.right - viewport.left,
                          viewport.bottom - viewport.top,
                          0, 0, p.frame_buffer.width, p.frame_buffer.height,
                          p.frame_buffer.data,
                          &bi,
                          0, SRCCOPY);


            if (!SwapBuffers(dc)) {
                assert(false);
            }
            ReleaseDC(p.hwnd, dc);

        }

        Sleep(16);
    }

    return 0;
}
