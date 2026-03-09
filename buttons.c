#include <windows.h>

HWND CreateButton(HWND parent, const char* text, int x, int y, int w, int h, int id)
{
    return CreateWindow(
        L"BUTTON",
        text,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x, y, w, h,
        parent,
        (HMENU)id,
        GetModuleHandle(NULL),
        NULL
    );
}
