#pragma once

enum struct Key {
    NONE = 0,
    // NOTE: alnum keys will be the character value
    SPACE = 32,
    a = 97,
    b,
    c,
    d,
    e,
    f,
    g,
    h,
    i,
    j,
    k,
    l,
    m,
    n,
    o,
    p,
    q,
    r,
    s,
    t,
    u,
    v,
    w,
    x,
    y,
    z,
    CURLY_OPEN,
    PIPE,
    CURLY_CLOSE,
    TILDE,
    BACKSPACE,
    // NOTE: start at 128 that it dont collides with ascii table
    LEFT,
    RIGHT,
    UP,
    DOWN,
    ESCAPE,
    RETURN,

};


// NOTE: wrapper structs to get some strong type checking
// TODO: maybe its better to use int masks for these flags
#define KEY_MOD_CTRL ctrl_flag{true}
#define KEY_MOD_SHIFT shift_flag{true}

struct ctrl_flag {
    bool value;
};

struct shift_flag {
    bool value;
};

struct KeyDef {
    Key key;
    ctrl_flag ctrl = {false};
    shift_flag shift = {false};

    KeyDef(): key(Key::NONE){};
    KeyDef(Key _key): key(_key){};

    bool operator==(const KeyDef &other) const {
        return this->ctrl.value == other.ctrl.value && this->key == other.key && this->shift.value == other.shift.value;
    }
};

// NOTE: Convenient operators for config.h
// with this you can write Key::j + CTRL
KeyDef operator+(const Key a, const ctrl_flag b);
KeyDef operator+(const Key a, const shift_flag b);
KeyDef operator+(const KeyDef a, const ctrl_flag b);
KeyDef operator+(const KeyDef a, const shift_flag b);

