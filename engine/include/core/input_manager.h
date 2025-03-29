#pragma once

#include <windows.h>
#include "utils/forward.h"

class InputManager
{
public:
    InputManager();

    bool isKeyDown(KeyCode key);
    bool isKeyUp(KeyCode key);
    bool isKeyHeld(KeyCode key);

    bool isMouseDown(KeyCode key);
    bool isMouseUp(KeyCode key);
    bool isMouseHeld(KeyCode key);

    float getMouseDeltaX() const;
    float getMouseDeltaY() const;

    void updateStates();

private:
    BYTE m_keyStates[256] = {};
    BYTE m_oldKeyStates[256] = {};

    POINT m_mousePosition = {};
    POINT m_oldMousePosition = {};
};
