#include "core/input_manager.h"
#include <cstring>

InputManager::InputManager()
{
    memset(m_keyStates, 0, sizeof(m_keyStates));
    memset(m_oldKeyStates, 0, sizeof(m_oldKeyStates));

    GetCursorPos(&m_mousePosition);
    m_oldMousePosition = m_mousePosition;
}

bool InputManager::isKeyDown(KeyCode key)
{
    int keyIndex = getWinKeyCode(key);
    return (m_keyStates[keyIndex] & 0x80) && !(m_oldKeyStates[keyIndex] & 0x80);
}

bool InputManager::isKeyUp(KeyCode key)
{
    int keyIndex = getWinKeyCode(key);
    return !(m_keyStates[keyIndex] & 0x80) && (m_oldKeyStates[keyIndex] & 0x80);
}

bool InputManager::isKeyHeld(KeyCode key)
{
    int keyIndex = getWinKeyCode(key);
    return (m_keyStates[keyIndex] & 0x80);
}

bool InputManager::isMouseDown(KeyCode key)
{
    int keyIndex = getWinKeyCode(key);
    return (m_keyStates[keyIndex] & 0x80) && !(m_oldKeyStates[keyIndex] & 0x80);
}

bool InputManager::isMouseUp(KeyCode key)
{
    int keyIndex = getWinKeyCode(key);
    return !(m_keyStates[keyIndex] & 0x80) && (m_oldKeyStates[keyIndex] & 0x80);
}

bool InputManager::isMouseHeld(KeyCode key)
{
    int keyIndex = getWinKeyCode(key);
    return (m_keyStates[keyIndex] & 0x80);
}

float InputManager::getMouseDeltaX() const
{
    return static_cast<float>(m_mousePosition.x - m_oldMousePosition.x);
}

float InputManager::getMouseDeltaY() const
{
    return static_cast<float>(m_mousePosition.y - m_oldMousePosition.y);
}

void InputManager::updateStates()
{
    memcpy(m_oldKeyStates, m_keyStates, sizeof(m_keyStates));
    GetKeyboardState(m_keyStates);

    m_oldMousePosition = m_mousePosition;
    GetCursorPos(&m_mousePosition);
}
