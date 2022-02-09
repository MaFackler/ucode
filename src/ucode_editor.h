#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

#include "ucode_utils.h"

using std::vector;
using std::string;

enum struct EditorState {
    BUFFER_NORMAL,
    BUFFER_INSERT,
    OPEN_DIRECTORY,
};

struct Editor {

private:
    EditorState _state;
public:
    bool quit = false;
    int col = 0;
    int row = 0;
    int screen_columns = 0;
    int screen_rows = 0;
    int scroll_offset = 0;
    string buffer_name;
    // TODO: some kind of Buffer Structure like GapBuffer?
    vector<string> lines;


    string current_folder{""};
    index_vector<std::filesystem::path> files;
    // TODO: open_dir; open_file should always accept absolute path???
    void open_dir(const char *dirname);
    void open_file(const char *filename);
    void open_target(const char *targetname);
    void save_file();
    void move_cursor(int dx, int dy);
    void move_cursor_abs(int dx, int dy);
    void move_cursor_end();
    int get_end_col(int row);
    void goto_state(EditorState state);
    void insert_char(char c);
    void insert_new_line();
    void remove_char();

    string get_current_filename();

    inline EditorState state() { return this->_state; }
};
