#include <iostream>

#include "ucode_terminal.h"

using std::cout;

static Terminal t;

void exit_handler() {
    t.shutdown();
}

int main(int, char **) {
    t.init();

    std::atexit(exit_handler);
    while (1) {
        char c = 0;
        read(STDIN_FILENO, &c, 1);
        if (c == 0)
            continue;
        if (iscntrl(c)) {
            cout << c << "\r\n";
        } else {
            cout << c << " " << (int) c << "\r\n";
        }
        if (c == 'q')
            break;
    }
    return 0;
}
