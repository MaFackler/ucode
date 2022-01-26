#include <doctest.h>
#include "ucode_editor.h"

namespace fs = std::filesystem;

void Editor::open_dir(const char *dirname) {
    this->state = EditorState::OPEN_DIRECTORY;

    if (std::string(dirname) != ".") {
        this->relative_dir = fs::path(this->relative_dir) / fs::path(dirname);
    }

    string abspath = this->get_current_folder();
    this->files.clear();
    this->files.emplace_back("..");
    for (auto &f: std::filesystem::directory_iterator(abspath)) {
        //this->files.emplace_back(f.path().string().substr(0));
        this->files.emplace_back(f.path().filename());
    }
}

void Editor::open_file(const char *filename) {
    this->state = EditorState::BUFFER;
    this->lines.clear();
    fs::path abspath = fs::path(this->get_current_folder());
    abspath /= filename;
    std::ifstream fp(abspath.string());
    if (fp.is_open()) {
        string line;
        while (std::getline(fp, line)) {
            this->lines.push_back(line);
        }
        this->buffer_name = string(filename);
    }
}

void Editor::save_file() {
    std::ofstream out(this->buffer_name.c_str());
    size_t len = this->lines.size();
    for (size_t i = 0; i < len; ++i) {
        auto &line = this->lines[i];
        out.write(&line[0], line.size());
        if (i < len - 1) {
            // TODO: line endings??
            out.write("\n", 1);
        }
    }
    out.close();
}


void Editor::move_cursor(int dx, int dy) {
    int new_col = 0;
    int new_row = 0;
    int num_lines = static_cast<int>(this->lines.size());
    if (num_lines) {
        new_row = std::clamp(this->row + dy, 0, num_lines - 1);
        int num_chars = static_cast<int>(this->lines[new_row].size());
        if (num_chars) {
            new_col = std::clamp(this->col + dx, 0, num_chars);
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
        CHECK(e.col == 5);
        CHECK(e.row == 1);

        e.move_cursor(0, 1);
        CHECK(e.col == 0);
        CHECK(e.row == 2);
    }
}

void Editor::insert_char(char c) {
    if (this->lines.size() == 0) {
        this->lines.emplace_back();
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
    CHECK(e.col == 2);
    CHECK(e.lines[0] == "hl");

    e.move_cursor(-1, 0);
    CHECK(e.col == 1);
    e.insert_char('e');
    CHECK(e.lines[0] == "hel");
}

void Editor::insert_new_line() {

    if (this->lines.size()) {
        string &line = this->lines[this->row];

        string to_insert("");
        auto pos = this->lines.begin() + this->row;
        if (this->col == 0) {
            // Start of line
            //this->lines.emplace(this->lines.begin() + this->row);
        } else if (static_cast<int>(line.size()) == this->col) {
            // End of line
            //this->lines.insert(this->lines.begin() + this->row + 1, "");
            pos++;
        } else {
            // Middle line
            to_insert = line.substr(this->col);
            this->lines[this->row] = line.substr(0, this->col);
            pos++;
        }
        this->lines.insert(pos, to_insert);
        this->row++;
        this->col = 0;
    } else {
        this->lines.emplace_back();
    }
}

TEST_CASE("Editor::insert_new_line") {

    SUBCASE("empty lines") {
        Editor e;
        e.insert_new_line();
        CHECK(e.lines.size() == 1);
        CHECK(e.lines[0] == "");
        CHECK(e.row == 0);
        CHECK(e.col == 0);

        e.insert_new_line();
        CHECK(e.lines.size() == 2);
        CHECK(e.row == 1);
    }

    auto setup = [](Editor &e) {
        e.insert_char('H');
        e.insert_char('e');
        e.insert_char('l');
        e.insert_char('l');
        e.insert_char('o');
        CHECK(e.lines.size() == 1);
        CHECK(e.lines[0] == "Hello");
        CHECK(e.col == 5);
        CHECK(e.row == 0);
    };

    SUBCASE("line with content cursor at the start") {
        Editor e;
        setup(e);

        e.move_cursor(-5, 0);
        e.insert_new_line();
        CHECK(e.lines.size() == 2);
        CHECK(e.lines[0] == "");
        CHECK(e.lines[1] == "Hello");
        CHECK(e.col == 0);
        CHECK(e.row == 1);
    }

    SUBCASE("line with content cursor in the middle") {
        Editor e;
        setup(e);

        e.move_cursor(-4, 0);
        CHECK(e.col == 1);
        CHECK(e.row == 0);

        e.insert_new_line();
        CHECK(e.col == 0);
        CHECK(e.row == 1);
        CHECK(e.lines.size() == 2);
        CHECK(e.lines[0] == "H");
        CHECK(e.lines[1] == "ello");

    }

    SUBCASE("line with content curser at the end") {
        Editor e;
        setup(e);

        e.insert_new_line();
        CHECK(e.lines.size() == 2);
        CHECK(e.lines[1] == "");
        CHECK(e.row == 1);
        CHECK(e.col == 0);

        e.insert_new_line();
        CHECK(e.lines.size() == 3);
        CHECK(e.lines[0] == "Hello");
        CHECK(e.lines[1] == "");
        CHECK(e.lines[2] == "");
    }

}

string Editor::get_current_folder() {
    fs::path abspath = fs::path(this->cwd);
    if (this->relative_dir != ".") {
        abspath /= fs::path(this->relative_dir);
    }
    return abspath.string();
}

TEST_CASE("Editor::get_current_folder") {
    Editor e;
    e.cwd = ".";
    e.relative_dir = ".";

    CHECK(e.get_current_folder() == ".");
}

