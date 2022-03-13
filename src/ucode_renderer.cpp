
#ifdef _WIN32
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_DESTROY: {
            PostQuitMessage(0); 
        } break;
        default:
            break;

    }
    return DefWindowProcA(hwnd, msg, wparam, lparam);
}
#endif

void Renderer::init() {
#ifdef _WIN32
    HINSTANCE instance = (HINSTANCE) GetModuleHandle(NULL);
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = "UCODE_WINDOW_CLASS";
    RegisterClassA(&wc);
    this->hwnd = CreateWindowExA(0,
                               wc.lpszClassName,
                               "UCode",
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               NULL,
                               NULL,
                               instance,
                               NULL);

    if (this->hwnd == NULL) {
        // TODO: handle
    }
    ShowWindow(this->hwnd, SW_SHOW);
    GetClientRect(this->hwnd, &this->rect);
#endif
}

void Renderer::handle_input() {
    this->msg = {};
    KeyDef def = {};
    while (PeekMessageA(&this->msg, NULL, 0, 0, PM_REMOVE)) {
        switch (this->msg.message) {
            case WM_QUIT:{
                this->close = true;
            } break;
            case WM_PAINT: {
                //PAINTSTRUCT ps = {};
                //HDC hdc = BeginPaint(this->hwnd, &ps);
                ////FillRect(hdc, &rect, 0);
                //DrawTextA(hdc, "foo", 3, &rect, DT_LEFT);
                //EndPaint(this->hwnd, &ps);
            } break;
            case WM_CHAR: {
                def.key = (Key) ((char) this->msg.wParam);
                this->keys.push_back(def);
            } break;
            case WM_KEYDOWN: {
                if (this->msg.wParam == VK_CONTROL) {
                    def.ctrl = {true};
                }
            } break;
            case WM_KEYUP: {
                if (this->msg.wParam == VK_CONTROL) {
                    def.ctrl = {false};
                }
            } break;
        }
        TranslateMessage(&this->msg);
        DispatchMessage(&this->msg);
    }
}

void Renderer::begin() {
    this->dc = GetDC(this->hwnd);
    GetTextExtentPoint32A(this->dc, "A", 1, &this->size);
    this->line_counter = 0;
}

void Renderer::end() {
    ReleaseDC(this->hwnd, this->dc);
}

void Renderer::set_color(enum RenderColor color) {
    auto c = this->_get_color(color);
    SetTextColor(this->dc, c);
}

void Renderer::write(const char *text) {
    RECT r;
    r.left = this->rect.left + (this->column_counter * this->size.cx);
    r.right = this->rect.right;
    r.top = this->line_counter * this->size.cy;
    r.bottom = this->rect.bottom;

    size_t n = strlen(text);
    this->column_counter += n;
    DrawTextA(this->dc, text, n, &r, DT_LEFT);
}

void Renderer::write_new_line() {
    this->line_counter++;
    this->column_counter = 0;
}

COLORREF Renderer::_get_color(enum RenderColor color) {
    COLORREF c = 0x00000000;
    switch (color) {
        case RenderColor::RED:
            c = 0x00FF0000;
            break;
        case RenderColor::GREEN:
            c = 0x0000FF00;
            break;
        case RenderColor::BLUE:
            c = 0x000000FF;
            break;
        default:
            break;
    }
    return c;
}

void Renderer::set_background_color(enum RenderColor color) {
    auto c = this->_get_color(color);
    SetBkColor(this->dc, c);
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
    //InvalidateRect(this->hwnd, &this->rect, FALSE);
    this->keys.clear();
}
void Renderer::shutdown() {}
