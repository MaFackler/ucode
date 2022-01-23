#pragma once

struct ICommand {
    Key key;  // NOTE: key which led to this command / was pressed
    virtual void execute(Editor &e) = 0;
};

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

