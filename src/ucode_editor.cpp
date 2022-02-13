#include <doctest.h>
#include "ucode_editor.h"

namespace fs = std::filesystem;

void Editor::open_dir(const char *dirname) {
    this->goto_state(EditorState::OPEN_DIRECTORY);
    std::string d = std::string(dirname);
    if (d != ".") {
        if (d == "..") {
            this->current_folder = fs::path(this->current_folder).parent_path();
        } else {
            this->current_folder = fs::path(this->current_folder) / fs::path(d);
        }
    } 

    this->files.clear();
    string abspath = this->current_folder;
    if (abspath.empty())
        abspath = ".";
    else
        this->files.emplace_back("..");
    for (auto &f: std::filesystem::directory_iterator(abspath)) {
        //this->files.emplace_back(f.path().string().substr(0));
        this->files.emplace_back(f.path());
    }
}

void Editor::open_file(const char *filename) {
    this->scroll_offset = 0;
    this->goto_state(EditorState::BUFFER_NORMAL);
    this->lines.clear();
    this->token_lines.clear();

    string abspath = path_join(this->current_folder, filename);
    std::ifstream fp(abspath);
    if (fp.is_open()) {
        string line;
        while (std::getline(fp, line)) {
            auto &l = this->lines.emplace_back(line);
            auto &tokens = this->token_lines.emplace_back();
            Lexer::lex(l.c_str(), tokens);
        }
        this->buffer_name = string(filename);
    }
}

void Editor::open_target(const char *targetname) {
    string abspath = path_join(this->current_folder, targetname);
    if (std::filesystem::is_directory(abspath)) {
        this->open_dir(targetname);
    } else {
        this->open_file(targetname);
    }


}

void Editor::save_file() {
    std::ofstream out(this->get_current_filename());
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
    this->move_cursor_abs(this->col + dx, this->row + dy);
}

TEST_CASE("Editor::move_cursor") {

    SUBCASE("no contests") {
        Editor e;
        e.move_cursor(1, 2);
        CHECK(e.col == 0);
        CHECK(e.row == 0);
    }

    SUBCASE("with contents") {
        Editor e;
        e.goto_state(EditorState::BUFFER_NORMAL);
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

    SUBCASE("with contents and scroll") {
        Editor e;
        e.screen_rows = 2;
        e.scroll_offset = 0;
        e.lines.emplace_back("Line 1");
        e.lines.emplace_back("Line 2");
        e.lines.emplace_back("Line 3");

        // move down
        e.move_cursor(0, 1);
        CHECK(e.scroll_offset == 0);
        e.move_cursor(0, 1);
        CHECK(e.scroll_offset == 1);

        // move up again
        e.move_cursor(0, -1);
        CHECK(e.scroll_offset == 1);
        e.move_cursor(0, -1);
        CHECK(e.scroll_offset == 0);
    }
}

void Editor::move_cursor_abs(int x, int y) {
    int new_col = 0;
    int new_row = 0;
    int num_lines = static_cast<int>(this->lines.size());
    if (num_lines) {
        new_row = std::clamp(y, 0, num_lines - 1);
        int end_col = this->get_end_col(new_row);
        if (end_col) {
            new_col = std::clamp(x, 0, end_col);
        }
    }
    this->col = new_col;
    int delta = new_row - this->row;
    this->row = new_row;

    if (delta > 0 && this->row >= this->scroll_offset + this->screen_rows) {
        this->scroll_offset = this->row - this->screen_rows + 1;
    } else if (delta < 0 && this->row < this->scroll_offset) {
        this->scroll_offset = this->row;
    }
}

void Editor::move_cursor_end() {
    if (this->lines.size() > 0) {
        int dest = this->get_end_col(this->row);
        this->move_cursor_abs(dest, this->row);
    }
}

TEST_CASE("Editor::move_cursor_end") {
    Editor e;
    e.goto_state(EditorState::BUFFER_NORMAL);
    e.lines.emplace_back("hello");
    CHECK(e.row == 0);
    CHECK(e.col == 0);
    e.move_cursor_end();
    CHECK(e.col == 4);
    e.goto_state(EditorState::BUFFER_INSERT);
    e.move_cursor_end();
    CHECK(e.col == 5);
}

int Editor::get_end_col(int row) {
    int res = 0;
    if (row < static_cast<int>(this->lines.size())) {
        res = this->lines[row].size();
        if (res > 0 && this->_state == EditorState::BUFFER_NORMAL) {
            res--;
        }
    }
    return res;
}

void Editor::goto_state(EditorState state) {
    this->_state = state;
    if (this->_state == EditorState::BUFFER_NORMAL) {
        auto &line = this->lines[this->row];
        if (lines.size() && this->col == static_cast<int>(line.size())) {
            this->col = line.size() - 1;
        }
    }
}

void Editor::insert_char(char c) {
    if (this->lines.size() == 0) {
        this->lines.emplace_back();
        this->token_lines.emplace_back();
    }
    auto &s = this->lines[this->row];
    s.insert(s.begin() + this->col++, c);
    auto &t = this->token_lines[this->row];
    Lexer::lex(s.c_str(), t);
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
        int pos = this->row;
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
        this->lines.insert(this->lines.begin() + pos, to_insert);
        this->token_lines.emplace(this->token_lines.begin() + pos,
                                  vector<Token>());
        Lexer::lex(this->lines[pos].c_str(), this->token_lines[pos]);
        if (pos > 0) {
            Lexer::lex(this->lines[pos - 1].c_str(), this->token_lines[pos - 1]);
        }
        this->row++;
        this->col = 0;
    } else {
        this->lines.emplace_back();
        this->token_lines.emplace_back();
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

void Editor::remove_char() {
    if (this->lines.size()) {
        auto &line = this->lines[this->row];
        if (this->col > 0) {
            line.erase(--this->col, 1);
        }
    }
}

TEST_CASE("Editor::remove_char") {
    Editor e;
    e.goto_state(EditorState::BUFFER_INSERT);
    e.insert_char('h');
    e.insert_char('e');

    e.remove_char();
    CHECK(e.lines[0] == "h");
    CHECK(e.row == 0);
    CHECK(e.col == 1);

    e.remove_char();
    CHECK(e.lines[0] == "");
    CHECK(e.row == 0);
    CHECK(e.col == 0);
}

string Editor::get_current_filename() {
    return path_join(this->current_folder, this->buffer_name);
}

TEST_CASE("Editor::get_current_filename") {
    Editor e;
    e.buffer_name = "hello_world.txt";
    CHECK(e.get_current_filename() == "hello_world.txt");

    e.current_folder = "subdir";
    CHECK(e.get_current_filename() == "subdir/hello_world.txt");
}

