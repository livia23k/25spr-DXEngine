#pragma once

enum class KeyCode
{
    // letters
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // digits
    Num0, Num1, Num2, Num3, Num4,
    Num5, Num6, Num7, Num8, Num9,

    // function keys
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10,
    F11, F12, F13, F14, F15, F16, F17, F18, F19, F20,
    F21, F22, F23, F24,

    // special
    Space, Enter, Escape, Backspace, Tab,
    CapsLock, Shift, Ctrl, Alt, LeftSuper, RightSuper, Menu,

    // arrow
    Left, Right, Up, Down,

    // others
    PageUp, PageDown, Home, End, Insert, Delete,
    PrintScreen, ScrollLock, Pause,

    // mouse
    MouseLeft, MouseRight, MouseMiddle, 
};

#ifdef _WIN32
    #include <windows.h>
    inline int getWinKeyCode(KeyCode key)
    {
        // letters
        if (key >= KeyCode::A && key <= KeyCode::Z)
            return 'A' + static_cast<int>(key) - static_cast<int>(KeyCode::A);

        // digits
        if (key >= KeyCode::Num0 && key <= KeyCode::Num9)
            return '0' + static_cast<int>(key) - static_cast<int>(KeyCode::Num0);

        // function keys
        if (key >= KeyCode::F1 && key <= KeyCode::F24)
            return VK_F1 + static_cast<int>(key) - static_cast<int>(KeyCode::F1);

        // others
        switch (key)
        {
        case KeyCode::Space: return VK_SPACE;
        case KeyCode::Enter: return VK_RETURN;
        case KeyCode::Escape: return VK_ESCAPE;
        case KeyCode::Backspace: return VK_BACK;
        case KeyCode::Tab: return VK_TAB;
        case KeyCode::CapsLock: return VK_CAPITAL;
        case KeyCode::Shift: return VK_SHIFT;
        case KeyCode::Ctrl: return VK_CONTROL;
        case KeyCode::Alt: return VK_MENU;
        case KeyCode::LeftSuper: return VK_LWIN;
        case KeyCode::RightSuper: return VK_RWIN;
        case KeyCode::Menu: return VK_APPS;

        case KeyCode::Left: return VK_LEFT;
        case KeyCode::Right: return VK_RIGHT;
        case KeyCode::Up: return VK_UP;
        case KeyCode::Down: return VK_DOWN;

        case KeyCode::PageUp: return VK_PRIOR;
        case KeyCode::PageDown: return VK_NEXT;
        case KeyCode::Home: return VK_HOME;
        case KeyCode::End: return VK_END;
        case KeyCode::Insert: return VK_INSERT;
        case KeyCode::Delete: return VK_DELETE;

        case KeyCode::PrintScreen: return VK_SNAPSHOT;
        case KeyCode::ScrollLock: return VK_SCROLL;
        case KeyCode::Pause: return VK_PAUSE;

        case KeyCode::MouseLeft: return VK_LBUTTON;
        case KeyCode::MouseRight: return VK_RBUTTON;
        case KeyCode::MouseMiddle: return VK_MBUTTON;
        
        default: return -1;
        }
    }
#endif