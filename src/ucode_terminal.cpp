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
