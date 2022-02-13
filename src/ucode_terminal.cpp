#include "ucode_terminal.h"


void Terminal::init() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &this->orginal);
    t = this->orginal;

    // ICANON: byte by byte
    // ISIG: Ctrl-C and Ctrl-Z
    // IEXTEN: Ctrl-V
    t.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    // IXON: Ctrl-S Ctrl-q
    // ICRNL: Ctrl-M
    t.c_iflag &= ~(IXON | ICRNL);
    // OPOST: Output processing
    t.c_oflag &= ~(OPOST);
    t.c_cc[VMIN] = 0;
    t.c_cc[VTIME] = 1; // 1/10 - 100ms
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &t);
}

void Terminal::shutdown() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &this->orginal);
}

char Terminal::read_char() {
    char res;
    ::read(STDIN_FILENO, &res, 1);

#if 0
    if (res == '\x1b') {
        char sequence[3];
        if (::read(STDINT_FILENO, &sequence[0], 1) != 1) {
            return res;
        }
        if (::read(STDINT_FILENO, &sequence[1], 1) != 1) {
            return res;
        }

        if (seq[0] == '[') {
            // TODO: continue handling special keys
        }
    }
#endif
    return res;
}

void Terminal::clear_screen() {
    this->write_escape("2J");
}

void Terminal::clear_line() {
    this->write_escape("K");
}

void Terminal::reset_cursor() {
    this->write_escape("H");
}

void Terminal::set_color(TerminalColor color) {
    std::stringstream ss;
    ss << static_cast<int>(color) << "m";
    this->write_escape(ss.str().c_str());
}

void Terminal::set_invert_color(bool value) {
    if (value) {
        this->write_escape("7m");
    } else {
        this->write_escape("m");
    }
}

void Terminal::set_cursor_visibility(bool visible) {
    if (visible) {
        this->write_escape("?25h");
    } else {
        this->write_escape("?25l");
    }
}

void Terminal::set_cursor_pos(int col, int row) {
    // TODO: copies
    std::stringstream ss;
    ss << row + 1 << ";" << col + 1 << "H";
    this->write_escape(ss.str().c_str());
}

void Terminal::write(char c) {
    this->buffer += c;
}

void Terminal::write(const char *s) {
    this->buffer += s;
}

void Terminal::write(std::string_view &sv) {
    this->buffer += sv;
}

void Terminal::write_escape(const char *s) {
    this->write("\x1b[");
    this->write(s);
}

void Terminal::write_new_line() {
    this->write("\r\n");
}


void Terminal::flush() {
    ::write(STDOUT_FILENO, this->buffer.c_str(), this->buffer.size());
    this->buffer.clear();
}

std::tuple<int, int> Terminal::get_window_size() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1) {
        // TODO: error handling
    }
    return {ws.ws_col, ws.ws_row};
}

std::tuple<int, int> Terminal::get_cursor_pos() {
    char buf[32];
    ::write(STDOUT_FILENO, "\x1b[6n", 4);
    unsigned int i = 0;
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }

    buf[i] = 0;
    int rows = 0, cols = 0;
    std::sscanf(&buf[2], "%d;%d", &rows, &cols);
    return std::make_tuple(cols, rows);
}

