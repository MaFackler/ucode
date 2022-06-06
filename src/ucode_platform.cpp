 #include "ucode_platform.h"

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

void Platform::open_window(int width, int height) {
#ifdef _WIN32
    HINSTANCE instance = (HINSTANCE) GetModuleHandle(NULL);
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = "UCODE_WINDOW_CLASS";
    wc.style = CS_OWNDC;
    //wc.hbrBackground
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);
    this->hwnd = CreateWindowExA(0,
                               wc.lpszClassName,
                               "UCode",
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                               NULL,
                               NULL,
                               instance,
                               NULL);

    if (this->hwnd == NULL) {
        // TODO: handle
    }
    this->dc = GetDC(this->hwnd);
    if (!this->dc) {
        assert(false);
    }

    PIXELFORMATDESCRIPTOR pf = {0};
    pf.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pf.nVersion = 1;
    pf.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pf.iPixelType = PFD_TYPE_RGBA;
    pf.cColorBits = 32;
    pf.cDepthBits = 24;
    pf.cStencilBits = 8;
    pf.iLayerType = PFD_MAIN_PLANE;

    int pixel_format = ChoosePixelFormat(this->dc, &pf);
    if (!pixel_format) {
        assert(false);
    }

    if (!SetPixelFormat(this->dc, pixel_format, &pf)) {
        assert(false);
    }


    this->frame_buffer.width = width;
    this->frame_buffer.height = height;
    this->frame_buffer.data = (rgba8 *) this->alloc(width * height * sizeof(rgba8));

    ShowWindow(this->hwnd, SW_SHOW);
    GetClientRect(this->hwnd, &this->rect);
    
    this->font = CreateFontA(18, 0, 0, 0, 300,
                             false, false, false,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY, DEFAULT_PITCH, "Consolas");
    GetTextExtentPoint32A(this->dc, "A", 1, &this->size);
    //ReleaseDC(this->hwnd, this->dc);



#endif
}

void Platform::begin() {
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

    this->dc = GetDC(this->hwnd);
    SelectObject(this->dc, font);
}

void* Platform::alloc(size_t n) {
    void *res = VirtualAlloc(NULL, n, MEM_COMMIT, PAGE_READWRITE);
    assert(res);
    return res;
}

void Platform::set_color(unsigned int color) {
    //SetTextColor(this->dc, color);
}

void Platform::set_background_color(unsigned int color) {
}

void Platform::draw_text(float x, float y, const char *text, size_t n) {
    RECT r;
    r.left = this->rect.left + x;
    r.right = this->rect.right;
    r.top = y;
    r.bottom = this->rect.bottom;

    DrawTextA(this->dc, text, n, &r, DT_LEFT);
}

void Platform::end() {
    ReleaseDC(this->hwnd, this->dc);
    this->keys.clear();
}
