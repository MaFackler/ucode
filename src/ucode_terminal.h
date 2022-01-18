#pragma once
#include <termios.h>
#include <unistd.h>

struct Terminal {

    void init();
    void shutdown();
private:
    struct termios orginal;
};

