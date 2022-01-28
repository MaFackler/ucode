
struct CmdOpenDirectory: ICommand {
    void execute(Editor &e) {
        e.open_dir(".");
    }
};

template<int dy>
struct CmdMoveSelection: ICommand {
    void execute(Editor &e) {
        e.files.move_index(dy);
    };
};


void Init(state_command_map &keybindings) {
    auto *quit = new CmdQuit();
    auto *goto_buffer_normal = new CmdGotoState<EditorState::BUFFER_NORMAL>();
    auto *cursor_left = new CmdMoveCursor<-1, 0>();
    auto *cursor_right = new CmdMoveCursor<1, 0>();
    auto *cursor_up = new CmdMoveCursor<0, -1>();
    auto *cursor_down = new CmdMoveCursor<0, 1>();

    {
        // BUFFER NORMAL
        command_map &s = keybindings[EditorState::BUFFER_NORMAL];
        s[Key::LEFT] = cursor_left;
        s[Key::RIGHT] = cursor_right; 
        s[Key::DOWN] = cursor_down;
        s[Key::UP] = cursor_up; 
        s[Key::h] = cursor_left;
        s[Key::l] = cursor_right;
        s[Key::j] = cursor_down;
        s[Key::k] = cursor_up;
        s[Key::RETURN] = new CmdInsertLine();
        s[Key::i] = new CmdGotoState<EditorState::BUFFER_INSERT>();

        s[Key::q + MOD_CTRL] = quit;
        s[Key::o + MOD_CTRL] = new CmdOpenDirectory();
        s[Key::s + MOD_CTRL] = new CmdSaveFile();
    }

    {
        // BUFFER INSERT
        command_map &s = keybindings[EditorState::BUFFER_INSERT];
        s[Key::ESCAPE] = goto_buffer_normal;

        s[Key::q + MOD_CTRL] = quit;
    }

    {
        // OPEN_DIRECTORY
        command_map &s = keybindings[EditorState::OPEN_DIRECTORY];
        s[Key::ESCAPE] = goto_buffer_normal;
        s[Key::j] = new CmdMoveSelection<1>();
        s[Key::k] = new CmdMoveSelection<-1>();
        s[Key::RETURN] = new CmdOpenTarget();
        s[Key::q + MOD_CTRL] = quit;
    }

}
