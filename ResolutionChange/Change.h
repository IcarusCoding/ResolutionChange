#pragma once

#include <windows.h>

VOID ChangeResolution(DWORD width, DWORD height, INT index = -1);
VOID RegisterKey(DWORD id, DWORD mod, DWORD vk);
BOOL WINAPI CLIHandler(DWORD cEvent);