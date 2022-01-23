#pragma once
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using std::vector;
using std::string;

struct Editor {
    int col = 0;
    int row = 0;
    int screen_columns = 0;
    int screen_rows = 0;

    string buffer_name;
    // TODO: some kind of Buffer Structure like GapBuffer?
    vector<string> lines;

    void open_file(const char *filename);
    void save_file();
    void move_cursor(int dx, int dy);
    void insert_char(char c);
    void insert_new_line();
};
