#pragma once
#include "ucode_input.h"
#include "ucode_editor.h"


struct ICommand {
    Key key;  // NOTE: key which led to this command / was pressed
    virtual void execute(Editor &e) = 0;
};

struct KeyDefHash {
    size_t operator()(const KeyDef &p) const {
        auto h1 = std::hash<int>()(static_cast<int>(p.key));
        auto h2 = std::hash<bool>()(p.ctrl);
        return h1 ^ h2;
    }
};
typedef std::unordered_map<KeyDef, ICommand*, KeyDefHash> command_map;

struct CmdMoveCursor: ICommand {
    int dx;
    int dy;
    CmdMoveCursor(int dx, int dy): dx(dx), dy(dy) { }

    void execute(Editor &e) {
        e.move_cursor(dx, dy);
    }
};

struct CmdQuit: ICommand {
    void execute(Editor &e) {
        e.quit = true;
    }
};

struct CmdInsertLine: ICommand {
    void execute(Editor &e) {
        e.insert_new_line();
    }
};

