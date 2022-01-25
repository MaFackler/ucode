
struct CmdOpenDirectory: ICommand {
    void execute(Editor &e) {
        e.open_dir(".");
    }
};

template<int dy>
struct CmdSelectFile: ICommand {
    void execute(Editor &e) {
        e.file_index += dy;
    };
};

void Init(state_command_map &keybindings) {

    // BUFFER
    command_map &state_buffer = keybindings[EditorState::BUFFER];
    state_buffer[Key::LEFT] = new CmdMoveCursor<-1, 0>();
    state_buffer[Key::RIGHT] = new CmdMoveCursor<1, 0>();
    state_buffer[Key::DOWN] = new CmdMoveCursor<0, 1>();
    state_buffer[Key::UP] = new CmdMoveCursor<0, -1>();
    state_buffer[Key::q + MOD_CTRL] = new CmdQuit();
    state_buffer[Key::o + MOD_CTRL] = new CmdOpenDirectory();
    state_buffer[Key::o] = new CmdOpenDirectory();
    state_buffer[Key::RETURN] = new CmdInsertLine();

    // OPEN_DIRECTORY
    command_map &state_open_directory = keybindings[EditorState::OPEN_DIRECTORY];
    state_open_directory[Key::ESCAPE] = new CmdGotoState<EditorState::BUFFER>;
    state_open_directory[(Key) 'j'] = new CmdSelectFile<1>();
    state_open_directory[Key::k] = new CmdSelectFile<-1>();

}
