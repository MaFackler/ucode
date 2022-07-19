#pragma once

#define MF_FONT_IMPLEMENTATION
#include "mf_font.h"

struct Renderer {

    mffo_font font = {};

    void init();
    void clear();
};

