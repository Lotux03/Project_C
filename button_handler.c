#include <windows.h>

typedef void (*ButtonFunc)(void);

typedef struct {
    int id;
    ButtonFunc func;
} ButtonMap;

void Button1() { MessageBox(NULL, L"Button1", L"Info", MB_OK); }
void Button2() { MessageBox(NULL, L"Button2", L"Info", MB_OK); }

ButtonMap buttons[] = {
    {101, Button1},
    {102, Button2}
};

void HandleButtonClick(int id)
{
    for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++)
        if (buttons[i].id == id)
            buttons[i].func();
}