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
        auto h2 = std::hash<bool>()(p.ctrl.value);
        auto h3 = std::hash<bool>()(p.shift.value);
        return h1 ^ h2 ^ h3;
    }
};
typedef std::unordered_map<KeyDef, ICommand*, KeyDefHash> command_map;
typedef std::unordered_map<EditorState, command_map> state_command_map;


template<int dx, int dy>
struct CmdMoveCursor: ICommand {
    void execute(Editor &e) {
        e.move_cursor(dx, dy);
    }
};

template <EditorState s>
struct CmdGotoState: ICommand {
    void execute(Editor &e) {
        e.goto_state(s);
    };
};

struct CmdQuit: ICommand {
    void execute(Editor &e) {
        e.quit = true;
    }
};

struct CmdOpenTarget: ICommand {
    inline void execute(Editor &e) {
        auto &sel = e.files.get_index_item();
        e.open_target(sel.c_str());

        e.files.set_index(0);
    }
};


#define MAKE_SIMPLE_COMMAND(name, func) \
    struct name: ICommand { void execute(Editor &e) { e.func(); } };

MAKE_SIMPLE_COMMAND(CmdInsertLine, insert_new_line);
MAKE_SIMPLE_COMMAND(CmdSaveFile, save_file);
MAKE_SIMPLE_COMMAND(CmdMoveCursorEnd, move_cursor_end);
MAKE_SIMPLE_COMMAND(CmdRemoveChar, remove_char);
