#include <iostream>
#include <vector>

#include "Change.h"

using std::vector;
using std::wcout;

vector<DWORD> registeredKeys;

int main() {
	SetConsoleCtrlHandler(CLIHandler, TRUE);
	RegisterKey(1, MOD_CONTROL | MOD_ALT, 0x60);
	RegisterKey(2, MOD_CONTROL | MOD_ALT, 0x61);
	MSG msg;
	while (GetMessageA(&msg, nullptr, 0, 0) != 0) {
		if (msg.message == WM_HOTKEY) {
			switch (msg.wParam) {
				case 1: {
					ChangeResolution(1920, 1080);
					break;
				}
				case 2: {
					ChangeResolution(3840, 2160);
					break;
				}
			}
		}
	}
}

VOID _ChangeResolution(DWORD width, DWORD height, DISPLAY_DEVICE currAdapter, DWORD index) {
	DEVMODE dx = { 0 };
	if (EnumDisplaySettings(currAdapter.DeviceName, ENUM_CURRENT_SETTINGS, &dx) == 0) {
		wcout << "Could not retrieve settings for adapter " << currAdapter.DeviceName << "\n";
		return;
	}
	if (dx.dmPelsWidth == width && dx.dmPelsHeight == height) {
		wcout << "The resolution of adapter " << currAdapter.DeviceName << " is already " << width << "x" << height << "\n";
		return;
	}
	dx.dmPelsWidth = width;
	dx.dmPelsHeight = height;
	dx.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;
	if (ChangeDisplaySettingsEx(currAdapter.DeviceName, &dx, NULL, CDS_TEST, NULL)) {
		wcout << width << "x" << height << " is not supported by adapter " << currAdapter.DeviceName << "\n";
		return;
	}
	if (ChangeDisplaySettingsEx(currAdapter.DeviceName, &dx, NULL, CDS_UPDATEREGISTRY, NULL)) {
		wcout << "Could not change settings for adapter " << currAdapter.DeviceName << "\n";
		return;
	}
	wcout << "Successfully changed the resolution of adapter " << currAdapter.DeviceName << " to " << width << "x" << height << "\n";
}

VOID ChangeResolution(DWORD width, DWORD height, INT index) {
	DISPLAY_DEVICE currAdapter;
	currAdapter.cb = sizeof(currAdapter);
	if (index == -1) {
		for (DWORD i = 0; EnumDisplayDevices(nullptr, i, &currAdapter, 0); i++) {
			_ChangeResolution(width, height, currAdapter, i);
		}
		return;
	}
	if (EnumDisplayDevices(nullptr, index, &currAdapter, 0) == 0) {
		wcout << "Invalid index " << index << "\n";
		return;
	}
	_ChangeResolution(width, height, currAdapter, index);
}

VOID RegisterKey(DWORD id, DWORD mod, DWORD vk) {
	if (RegisterHotKey(nullptr, id, mod, vk) != 0) {
		wcout << "Registered hotkey ("<< id << ") with mod " << mod << " -> 0x" << std::hex << vk << std::dec << "\n";
	}
	else {
		wcout << "Failed to register hotkey (" << id << ") with mod " << mod << " -> 0x" << std::hex << vk << std::dec << "\n";
	}
}

BOOL WINAPI CLIHandler(DWORD cEvent) {
	switch (cEvent) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT: {
		std::cout << "Exiting...\n";
		Sleep(500);
		break;
	}
	}
	return FALSE;
}