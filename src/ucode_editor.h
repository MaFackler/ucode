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
    BUFFER,
    OPEN_DIRECTORY,
};

struct Editor {

    bool quit = false;
    int col = 0;
    int row = 0;
    int screen_columns = 0;
    int screen_rows = 0;

    string buffer_name;
    // TODO: some kind of Buffer Structure like GapBuffer?
    vector<string> lines;

    // NOTE this is the enum defined in config
    // its just an int to avoid dependency
    EditorState state;

    string relative_dir{""};
    index_vector<string> files;
    void open_dir(const char *dirname);
    void open_file(const char *filename);
    void save_file();
    void move_cursor(int dx, int dy);
    void insert_char(char c);
    void insert_new_line();

    string get_current_folder();
    string get_current_filename();

};
