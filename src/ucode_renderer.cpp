

void Renderer::init(size_t rows, size_t columns) {
    this->rows = rows;
    this->columns = columns;
    this->text_buffer_size = rows * columns;
    this->text_buffer = (char *) malloc(this->text_buffer_size);
}

TEST_CASE("Renderer::init") {
    Renderer r;
    r.init(10, 5);
    CHECK(r.text_buffer_size == 50);
    CHECK(r.rows == 10);
    CHECK(r.columns == 5);
}


void Renderer::push_text(int row, int col, const char *text, size_t n) {
    assert(this->num_commands < AMOUNT_COMMANDS);
    RenderCommand *cmd = &this->commands[this->num_commands++];
    cmd->type = CommandType::RENDER_TEXT;
    cmd->col = col;
    cmd->row = row;
    cmd->n = n;

    char *dest = this->text_buffer + (cmd->row * this->columns + cmd->col);
    memcpy(dest, text, n);
}

TEST_CASE("Renderer::push_text") {
    Renderer r;
    r.dx = 30;
    r.dy = 40;
    r.init(10, 10);
    CHECK(r.num_commands == 0);
    r.push_text(1, 2, "test", 4);

    CHECK(r.num_commands == 1);
    RenderCommand &cmd = r.commands[0];
    CHECK(cmd.type == CommandType::RENDER_TEXT);
    CHECK(cmd.row == 1);
    CHECK(cmd.col == 2);
}

COLORREF Renderer::_get_color(enum RenderColor color) {
    COLORREF c = 0x00000000;
    switch (color) {
        case RenderColor::RED:
            c = 0x000000FF;
            break;
        case RenderColor::GREEN:
            c = 0x0000FF00;
            break;
        case RenderColor::BLUE:
            c = 0x00FF0000;
            break;
        case RenderColor::WHITE:
            c = 0x00FFFFFF;
        default:
            break;
    }
    return c;
}

void Renderer::set_color(enum RenderColor color) {
    auto c = this->_get_color(color);
}

void Renderer::set_background_color(enum RenderColor color) {
    // auto c = this->_get_color(color);
    // SetBkColor(this->dc, c);
}



void Renderer::clear_line() {
}

tuple<int, int> Renderer::get_window_size() {
    tuple<int, int> res;
    return res;
}

void Renderer::clear_screen() {
}

void Renderer::flush() {
    // InvalidateRect(this->hwnd, &this->rect, FALSE);
    // this->keys.clear();
}
void Renderer::shutdown() {}
