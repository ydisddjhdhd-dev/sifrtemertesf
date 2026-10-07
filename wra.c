#include <windows.h>

#define IDC_BTN_DELETE 101
#define IDC_BTN_CREATE 102

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateWindowW(
                L"BUTTON", L"حذف تعريفات إنفيديا",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                50, 50, 200, 40,
                hwnd, (HMENU)IDC_BTN_DELETE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
            );

            CreateWindowW(
                L"BUTTON", L"إنشاء تعريفات جديدة إنفيديا",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                50, 110, 200, 40,
                hwnd, (HMENU)IDC_BTN_CREATE, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
            );
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDC_BTN_DELETE) {
                MessageBoxW(hwnd, L"تم إرسال أمر حذف تعريفات إنفيديا.", L"تنبيه", MB_OK | MB_ICONINFORMATION);
            } else if (LOWORD(wParam) == IDC_BTN_CREATE) {
                MessageBoxW(hwnd, L"تم إرسال أمر إنشاء تعريفات إنفيديا الجديدة.", L"تنبيه", MB_OK | MB_ICONINFORMATION);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = LNVIDIA_Control_Class;

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"إدارة تعريفات إنفيديا",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 310, 210,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { 0 };
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
