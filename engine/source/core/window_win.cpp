#include "core/window_win.h"

WindowWin::WindowWin(uint32_t width, uint32_t height, const std::string &title)
    : WindowBase(width, height, title)
{
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = m_className;

    if (!RegisterClassExA(&wc))
    {
        throw std::runtime_error("WindowWin::WindowWin: Failed to register window class!");
    }

    DWORD style = WS_OVERLAPPEDWINDOW;
    RECT rect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    AdjustWindowRect(&rect, style, FALSE);

    m_hwnd = CreateWindowExA(
        0, m_className, m_title.c_str(), style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, GetModuleHandle(nullptr), this);

    if (!m_hwnd)
    {
        throw std::runtime_error("WindowWin::WindowWin: Failed to create window!");
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
}

WindowWin::~WindowWin()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    UnregisterClassA(m_className, GetModuleHandle(nullptr));
}

void WindowWin::onProcessEvents() const
{
    MSG msg = {};
    while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

LRESULT CALLBACK WindowWin::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CREATE:
        if (auto *window = reinterpret_cast<WindowWin *>(reinterpret_cast<CREATESTRUCT *>(lparam)->lpCreateParams))
        {
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
            window->onCreate();
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        if (auto *window = reinterpret_cast<WindowWin *>(GetWindowLongPtr(hwnd, GWLP_USERDATA)))
        {
            if (window->m_destroyCallback)
            {
                window->m_destroyCallback();
            }
            window->onDestroy();
        }
        PostQuitMessage(0);
        break;

    case WM_SIZE:
    {
        if (auto *window = reinterpret_cast<WindowWin *>(GetWindowLongPtr(hwnd, GWLP_USERDATA)))
        {
            window->onResize(LOWORD(lparam), HIWORD(lparam));
            if (window->m_resizeCallback)
            {
                window->m_resizeCallback(window->m_width, window->m_height);
            }
        }
    }
    break;

    case WM_SETFOCUS:
        if (auto *window = reinterpret_cast<WindowWin *>(GetWindowLongPtr(hwnd, GWLP_USERDATA)))
        {
            window->onFocus();
        }
        break;

    case WM_KILLFOCUS:
        if (auto *window = reinterpret_cast<WindowWin *>(GetWindowLongPtr(hwnd, GWLP_USERDATA)))
        {
            window->onKillFocus();
        }
        break;

    case WM_MOUSEWHEEL:
        if (auto *window = reinterpret_cast<WindowWin *>(GetWindowLongPtr(hwnd, GWLP_USERDATA)))
        {
            if (window->m_zoomCallback)
            {
                window->m_zoomCallback(static_cast<float>(GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA));
            }
        }
        break;

    default:
        return DefWindowProc(hwnd, message, wparam, lparam);
    }
    return 0;
}
