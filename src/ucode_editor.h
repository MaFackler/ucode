#pragma once
#include <fstream>
#include <vector>
#include <string>

using std::vector;
using std::string;

struct Editor {
    int col = 0;
    int row = 0;
    int columns = 0;
    int rows = 0;

    vector<string> lines;

    void open_file(const char *filename);
};
