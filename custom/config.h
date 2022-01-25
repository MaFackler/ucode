

struct Commands {
    CmdMoveCursor move_col_right{1, 0};
    CmdMoveCursor move_col_left{-1, 0};
    CmdMoveCursor move_row_up{0, -1};
    CmdMoveCursor move_row_down{0, 1};
    CmdInsertLine insert_new_line;
    CmdQuit quit;
};

static Commands cmd;


void Init(command_map &keybindings) {

    // Commands

    keybindings[Key::LEFT] = &cmd.move_col_left;
    keybindings[Key::RIGHT] = &cmd.move_col_right;
    keybindings[Key::DOWN] = &cmd.move_row_down;
    keybindings[Key::UP] = &cmd.move_row_up;
    keybindings[KeyDef('q', true)] = &cmd.quit;
    keybindings[Key::RETURN] = &cmd.insert_new_line;
}
