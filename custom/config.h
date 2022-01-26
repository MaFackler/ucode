
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

struct CmdSelectFile: ICommand {
    void execute(Editor &e) {
        auto &sel = e.files.get_index_item();
        if (std::filesystem::is_directory(sel)) {
            e.open_dir(sel.c_str());
        } else {
            e.open_file(sel.c_str());
        }
    }
};

void Init(state_command_map &keybindings) {

    auto *quit = new CmdQuit();

    // BUFFER
    command_map &state_buffer = keybindings[EditorState::BUFFER];
    state_buffer[Key::LEFT] = new CmdMoveCursor<-1, 0>();
    state_buffer[Key::RIGHT] = new CmdMoveCursor<1, 0>();
    state_buffer[Key::DOWN] = new CmdMoveCursor<0, 1>();
    state_buffer[Key::UP] = new CmdMoveCursor<0, -1>();
    state_buffer[Key::RETURN] = new CmdInsertLine();

    // CTRL
    state_buffer[Key::q + MOD_CTRL] = quit;
    state_buffer[Key::o + MOD_CTRL] = new CmdOpenDirectory();
    state_buffer[Key::s + MOD_CTRL] = new CmdSaveFile();

    // OPEN_DIRECTORY
    command_map &state_open_directory = keybindings[EditorState::OPEN_DIRECTORY];
    state_open_directory[Key::ESCAPE] = new CmdGotoState<EditorState::BUFFER>;
    state_open_directory[Key::j] = new CmdMoveSelection<1>();
    state_open_directory[Key::k] = new CmdMoveSelection<-1>();
    state_open_directory[Key::RETURN] = new CmdSelectFile();
    state_open_directory[Key::q + MOD_CTRL] = quit;

}
