#include "ucode_editor.h"

void Editor::open_file(const char *filename) {
    std::ifstream fp(filename);
    if (fp.is_open()) {
        string line;
        while (std::getline(fp, line)) {
            this->lines.push_back(line);
        }
        this->buffer_name = string(filename);
    }
}
