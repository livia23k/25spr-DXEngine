#pragma once

#include "core/window_base.h"
#include <windows.h>

class WindowWin : public WindowBase
{
public:
    WindowWin(uint32_t width, uint32_t height, const std::string &title);
    virtual ~WindowWin();

    void onProcessEvents() const override;

    void *getNativeHandle() const override { return m_hwnd; }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

private:
    HWND m_hwnd;
    static constexpr LPCSTR m_className = "DX11WindowClass";
};
