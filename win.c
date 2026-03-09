#include <windows.h>
#include "win.h"
#include "button_handler.h"
#include "buttons.h"

#define BTN_OK     101
#define BTN_CANCEL 102

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int Create_Window(HINSTANCE hInstance, int nCmdShow, const wchar_t* window_title, const wchar_t* class_name) {
    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = class_name;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowExW(
        NULL,                           // Optional window styles.
        class_name,                     // Window class
        window_title,                           // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Run the message loop.

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        CreateButton(hwnd, "OK", 50, 50, 100, 30, BTN_OK);
        CreateButton(hwnd, "Cancel", 160, 50, 100, 30, BTN_CANCEL);
        break;

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        if (id == BTN_OK)
            MessageBox(hwnd, "OK Clicked", "Info", MB_OK);
        else if (id == BTN_CANCEL)
            MessageBox(hwnd, "Cancel Clicked", "Info", MB_OK);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
