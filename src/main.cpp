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
#define MF_OPENGL_IMPLEMENTATION
#include "mf_opengl.h"

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


void handle_key(mfp_platform &platform, Editor &e) {

    // NORMAL STATE
    if (platform.input.keys[MF_KEY_CTRL].down) {
        if (platform.input.keys['o'].pressed) {
            e.goto_state(EditorState::OPEN_DIRECTORY);
            e.open_target(".");
        }
    } else {
    }

    // OPEN_DIRECTORY STATE
}

void render_text(mffo_font *font, const char *text, float x, float y) {

    for (int i = 0; i < strlen(text); ++i) {
        auto ch = font->characters[text[i]];
        float ystart = y - ch.ybearing;
        glBegin(GL_QUADS);
            glTexCoord2f(ch.u0, ch.v0);
            glVertex2f(x + ch.xbearing, ystart);
            //glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(ch.u1, ch.v0);
            glVertex2f(x + ch.xbearing + ch.width, ystart);
            //glVertex2f(1.0f, -1.0f);
            glTexCoord2f(ch.u1, ch.v1);
            glVertex2f(x + ch.xbearing + ch.width, ystart + ch.height);
            //glVertex2f(1.0f, 1.0f);
            glTexCoord2f(ch.u0, ch.v1);
            glVertex2f(x + ch.xbearing, ystart + ch.height);
            //glVertex2f(-1.0f, 1.0f);
        glEnd();
        x += ch.advance;
    }
}


int main(int argc, char **argv) {

    LogInit();
    Debug << "Logging initialized";

    mfp_platform platform = {};
    mfp_init(&platform);
    const int width = 1600;
    const int height = 900;
    mfp_window_open(&platform, "UCode", 0, 0, width, height);

    UCODE_TEST;
    //t.init();
    std::atexit(exit_handler);

    mffo_font font;
#ifdef _WIN32
    const char *path = "c:/windows/fonts/arialbd.ttf";
#else
    const char *path = "/usr/share/fonts/ubuntu/Ubuntu-B.ttf";
#endif
    mffo_font_init(&font, path, 42.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    u32 texture_font = mfgl_texture_create_argb(FONT_ATLAS_DIM, FONT_ATLAS_DIM, font.data);
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

    mfgl_viewport_bottom_up(width, height);
    
    if (argc == 2) {
        e.open_file(argv[1]);
    }

    mfgl_texture_bind(texture_font);
    while (!e.quit && platform.window.isOpen) {

        mfp_begin(&platform);
        handle_key(platform, e);
        renderer.clear();

        // begin
        bool cursor_visible = true;
        auto state = e.state();

        if (state == EditorState::BUFFER_NORMAL || state == EditorState::BUFFER_INSERT) {
            size_t endline = e.lines.size();
            enum TokenType hl = TokenType::NONE;
            for (int row_index = e.scroll_offset; row_index < e.scroll_offset + e.screen_rows; ++row_index) {

                if (row_index < endline) {
                    auto &tokens = e.token_lines[row_index];
                    for (auto &token: tokens) {
                        if (token.type != hl) {
                        }
                        if (token.chars.size()) {
                        }
                    }
                } else {
                }
            }
        } else if (state == EditorState::OPEN_DIRECTORY) {
            float y = height - font.ascent;
            for (auto &file: e.files) {
                render_text(&font, file.string().c_str(), 0, y);
                y -= font.ascent - font.descent + font.linegap;
            }
        }


        mfp_end(&platform);
    }
    mfp_window_close(&platform);
    mfp_destroy(&platform);

    return 0;
}
