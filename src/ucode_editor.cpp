#include <doctest.h>
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


void Editor::move_cursor(int dx, int dy) {
    int new_col = 0;
    int new_row = 0;
    int num_lines = static_cast<int>(this->lines.size());
    if (num_lines) {
        new_row = std::clamp(this->row + dy, 0, num_lines - 1);
        int num_chars = static_cast<int>(this->lines[new_row].size());
        if (num_chars) {
            new_col = std::clamp(this->col + dx, 0, num_chars - 1);
        }
    }
    this->row = new_row;
    this->col = new_col;;
}

TEST_CASE("Editor::move_cursor") {
    Editor e;

    SUBCASE("no contests") {
        e.move_cursor(1, 2);
        CHECK(e.col == 0);
        CHECK(e.row == 0);
    }

    SUBCASE("with contents") {
        e.lines.push_back(string("Hello World 1"));
        e.lines.push_back(string("Hello"));
        e.lines.push_back(string(""));
        e.lines.push_back(string("Hello World 2"));
        e.move_cursor(7, 0);
        CHECK(e.col == 7);
        CHECK(e.row == 0);

        e.move_cursor(0, 1);
        CHECK(e.col == 4);
        CHECK(e.row == 1);

        e.move_cursor(0, 1);
        CHECK(e.col == 0);
        CHECK(e.row == 2);
    }
}

void Editor::insert_char(char c) {
    if (this->lines.size() == 0) {
        this->lines.emplace_back("");
    }
    auto &s = this->lines[this->row];
    s.insert(s.begin() + this->col++, c);
}

TEST_CASE("Editor::insert_char") {
    Editor e;
    CHECK(e.lines.size() == 0);
    CHECK(e.col == 0);

    e.insert_char('h');
    CHECK(e.lines[0].size() == 1);
    CHECK(e.col == 1);
    CHECK(e.lines[0] == "h");

    e.insert_char('l');
    CHECK(e.lines[0] == "hl");

    e.move_cursor(-1, 0);
    e.insert_char('e');
    CHECK(e.lines[0] == "hel");
}
     
