#pragma once

union rgba8 {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    uint32_t value32;
};

struct FrameBuffer {
    int width;
    int height;
    rgba8 *data;
};

struct Platform {

    bool close = false;
    HDC dc;
    RECT rect;
    HWND hwnd;
    MSG msg;
    std::vector<KeyDef> keys;
    SIZE size;
    HFONT font;
    FrameBuffer frame_buffer;

    void open_window(int width, int height);
    void begin();

    void *alloc(size_t n);
    void set_color(unsigned int color);
    void set_background_color(unsigned int color);
    void draw_text(float x, float y, const char *text, size_t n);
    void end();
};
