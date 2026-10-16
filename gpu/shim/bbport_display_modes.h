// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

/* Shared by the renderer and the launcher's capabilities helper. Only query display modes;
 * selecting an output resolution never changes the Windows desktop display mode. */
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

static inline void BbEnumerateDisplayModes(void (*add)(int, int, void*), void* context) {
    DISPLAY_DEVICEW device;
    for (DWORD adapter = 0;; ++adapter) {
        ZeroMemory(&device, sizeof(device));
        device.cb = sizeof(device);
        if (!EnumDisplayDevicesW(NULL, adapter, &device, 0)) break;
        if (!(device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)) continue;
        DEVMODEW mode;
        ZeroMemory(&mode, sizeof(mode));
        mode.dmSize = sizeof(mode);
        if (EnumDisplaySettingsW(device.DeviceName, ENUM_CURRENT_SETTINGS, &mode)) {
            add((int)mode.dmPelsWidth, (int)mode.dmPelsHeight, context);
        }
        for (DWORD index = 0;; ++index) {
            ZeroMemory(&mode, sizeof(mode));
            mode.dmSize = sizeof(mode);
            if (!EnumDisplaySettingsW(device.DeviceName, index, &mode)) break;
            if (mode.dmBitsPerPel >= 32) {
                add((int)mode.dmPelsWidth, (int)mode.dmPelsHeight, context);
            }
        }
    }
}
#endif
