#pragma once

#include "win.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef _WIN_H
#define _WIN_H

int Create_Window(HINSTANCE hInstance, int nCmdShow, const wchar_t* window_title, const wchar_t* class_name);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif