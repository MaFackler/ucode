#pragma once
#include "ucode_utils.h"
#include <set>
#include <string>
#include <filesystem>

using std::vector;
using std::string;
using std::set;

struct FileChooser {
    set<string> extensions;
    set<string> ignore;
    vector<std::filesystem::path> files;
    vector<size_t> filtered;

    string filter_string;

    void open_folder(const char *foldername);
    void filter();
};
