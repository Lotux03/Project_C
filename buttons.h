#pragma once

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef BUTTONS_H
#define BUTTONS_H

HWND CreateButton(HWND parent, const char* text, int x, int y, int w, int h, int id);

#endif