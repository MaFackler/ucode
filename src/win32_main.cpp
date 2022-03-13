#include <stdio.h>
#include <windows.h>


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

int main() {


    HINSTANCE instance = (HINSTANCE) GetModuleHandle(NULL);
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = "UCODE_WINDOW_CLASS";
    RegisterClassA(&wc);


    HWND hwnd = CreateWindowExA(0,
                               wc.lpszClassName,
                               "UCode",
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               NULL,
                               NULL,
                               instance,
                               NULL);

    if (hwnd == NULL) {
        return -1;
    }

    char buf[256] = {0};
    size_t buf_size = 0;

    ShowWindow(hwnd, SW_SHOW);
    bool quit = false;
    RECT rect;
    GetClientRect(hwnd, &rect);

    while (!quit) {

        MSG msg = {};
        while (!quit && PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            
            switch (msg.message) {
                case WM_CHAR: {
                    char c = (char) msg.wParam;
                    
                    if (c == 8) {
                        if (buf_size > 0) {
                            buf[buf_size--] = 0;
                        }
                    } else {
                        if (buf_size < 256) {
                            buf[buf_size++] = (char) msg.wParam;
                        }
                    }
                } break;
                case WM_QUIT: {
                    quit = true;
                } break;
                case WM_PAINT: {
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);


                    Rect r;
                    GetClientRect(hwnd, &r);
                    FillRect(hdc, &r, 0);
                    DrawTextA(hdc, &buf[0], buf_size, &r, DT_LEFT);
                    EndPaint(hwnd, &ps);
                } break;

            }


            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //InvalidateRect(hwnd, &rect, FALSE);
        Sleep(16);
    }

}
