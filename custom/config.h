
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

template<char c>
struct CmdInsertChar: ICommand {
    void execute(Editor &e) {
        e.insert_char(c);
    }
};

struct CmdChain: ICommand {
    std::vector<ICommand*> cmds;

    void addCommand(ICommand *cmd) {
        cmds.push_back(cmd);
    }

    void execute(Editor &e) override {
        for (auto *cmd: this->cmds) {
            cmd->execute(e);
        }
    }
};

void Keywords(vector<const char*> &keywords) {
    keywords.emplace_back("if");
    keywords.emplace_back("while");
    keywords.emplace_back("for");
    keywords.emplace_back("else");
}

void Init(state_command_map &keybindings) {
    auto *quit = new CmdQuit();
    auto *goto_buffer_normal = new CmdGotoState<EditorState::BUFFER_NORMAL>();
    auto *cursor_left = new CmdMoveCursor<-1, 0>();
    auto *cursor_right = new CmdMoveCursor<1, 0>();
    auto *cursor_up = new CmdMoveCursor<0, -1>();
    auto *cursor_down = new CmdMoveCursor<0, 1>();
    auto *cursor_page_down = new CmdMoveCursor<0, 5>();
    auto *cursor_page_up = new CmdMoveCursor<0, -5>();
    auto *cursor_end = new CmdMoveCursorEnd();

    auto *goto_insert = new CmdGotoState<EditorState::BUFFER_INSERT>(); 
    auto *insert_new_line = new CmdInsertLine();

    // VIM 'o' behaviour in NORMAL mode
    auto *new_line_and_goto_insert = new CmdChain();
    new_line_and_goto_insert->addCommand(goto_insert);
    new_line_and_goto_insert->addCommand(cursor_end);
    new_line_and_goto_insert->addCommand(insert_new_line);

    // VIM 'a' behaviour in NORMAL mode
    auto *append = new CmdChain();
    append->addCommand(goto_insert);
    append->addCommand(cursor_right);

    {
        // BUFFER NORMAL
        command_map &s = keybindings[EditorState::BUFFER_NORMAL];
        s[Key::LEFT] = cursor_left;
        s[Key::RIGHT] = cursor_right; 
        s[Key::DOWN] = cursor_down;
        s[Key::UP] = cursor_up; 
        s[Key::a] = append;
        s[Key::h] = cursor_left;
        s[Key::l] = cursor_right;
        s[Key::j] = cursor_down;
        s[Key::k] = cursor_up;
        s[Key::d + MOD_CTRL] = cursor_page_down;
        s[Key::u + MOD_CTRL] = cursor_page_up;
        s[Key::i] = goto_insert;
        s[Key::o] = new_line_and_goto_insert;

        s[Key::q + MOD_CTRL] = quit;
        s[Key::o + MOD_CTRL] = new CmdOpenDirectory();
        s[Key::s + MOD_CTRL] = new CmdSaveFile();
    }

    {
        // BUFFER INSERT
        command_map &s = keybindings[EditorState::BUFFER_INSERT];
        s[Key::ESCAPE] = goto_buffer_normal;
        s[Key::RETURN] = insert_new_line;
        s[Key::BACKSPACE] = new CmdRemoveChar();

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
