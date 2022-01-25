#pragma once

enum struct Key {
    NONE = 0,
    // NOTE: alnum keys will be the character value
    SPACE = 32,

    // NOTE: start at 128 that it dont collides with ascii table
    LEFT = 128,
    RIGHT,
    UP,
    DOWN,
    ESCAPE,
    RETURN,

};

struct KeyDef {
    Key key;
    bool ctrl = false;

    KeyDef(): key(Key::NONE), ctrl(false) {};
    KeyDef(Key _key): key(_key), ctrl(false) {};
    KeyDef(Key _key, bool _ctrl): key(_key), ctrl(_ctrl) {};
    KeyDef(char c, bool _ctrl): key((Key) c), ctrl(_ctrl) {};

    bool operator==(const KeyDef &other) const {
        return this->ctrl == other.ctrl && this->key == other.key;
    }
};
