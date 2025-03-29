#include "entity/entity_controllable.h"

ControllableEntity::ControllableEntity(ID3D11Device *device)
    : EntityBase(device), m_moveSpeed(5.f), m_rotationSpeed(30.f), m_isUnderControl(true)
{
}

void ControllableEntity::getMoveSpeed(float &moveSpeed) const { moveSpeed = m_moveSpeed; }
void ControllableEntity::getRotationSpeed(float &rotationSpeed) const { rotationSpeed = m_rotationSpeed; }
void ControllableEntity::setMoveSpeed(float moveSpeed) { m_moveSpeed = moveSpeed; }
void ControllableEntity::setRotationSpeed(float rotationSpeed) { m_rotationSpeed = rotationSpeed; }

void ControllableEntity::move(float deltaTime, MovementDirection direction)
{
    if (!m_isUnderControl)
        return;

    switch (direction)
    {
    case MovementDirection::Forward:
        moveForward(deltaTime);
        break;
    case MovementDirection::Backward:
        moveBackward(deltaTime);
        break;
    case MovementDirection::Left:
        moveLeft(deltaTime);
        break;
    case MovementDirection::Right:
        moveRight(deltaTime);
        break;
    case MovementDirection::Up:
        moveUp(deltaTime);
        break;
    case MovementDirection::Down:
        moveDown(deltaTime);
        break;
    default:
        break;
    }
}

void ControllableEntity::rotate(float deltaTime, RotationDirection direction)
{
    if (!m_isUnderControl)
        return;

    switch (direction)
    {
    case RotationDirection::YawLeft:
        rotateLeft(deltaTime);
        break;
    case RotationDirection::YawRight:
        rotateRight(deltaTime);
        break;
    case RotationDirection::PitchUp:
        rotateUp(deltaTime);
        break;
    case RotationDirection::PitchDown:
        rotateDown(deltaTime);
        break;
    default:
        break;
    }

    updateWorldMatrix();
    updateDirectionVectors();
}

void ControllableEntity::moveForward(float deltaTime)
{
    m_localPosition.x += m_front.x * m_moveSpeed * deltaTime;
    m_localPosition.y += m_front.y * m_moveSpeed * deltaTime;
    m_localPosition.z += m_front.z * m_moveSpeed * deltaTime;
}

void ControllableEntity::moveBackward(float deltaTime)
{
    m_localPosition.x -= m_front.x * m_moveSpeed * deltaTime;
    m_localPosition.y -= m_front.y * m_moveSpeed * deltaTime;
    m_localPosition.z -= m_front.z * m_moveSpeed * deltaTime;
}

void ControllableEntity::moveLeft(float deltaTime)
{
    m_localPosition.x -= m_right.x * m_moveSpeed * deltaTime;
    m_localPosition.y -= m_right.y * m_moveSpeed * deltaTime;
    m_localPosition.z -= m_right.z * m_moveSpeed * deltaTime;
}

void ControllableEntity::moveRight(float deltaTime)
{
    m_localPosition.x += m_right.x * m_moveSpeed * deltaTime;
    m_localPosition.y += m_right.y * m_moveSpeed * deltaTime;
    m_localPosition.z += m_right.z * m_moveSpeed * deltaTime;
}

void ControllableEntity::moveUp(float deltaTime)
{
    m_localPosition.x += m_up.x * m_moveSpeed * deltaTime;
    m_localPosition.y += m_up.y * m_moveSpeed * deltaTime;
    m_localPosition.z += m_up.z * m_moveSpeed * deltaTime;
}

void ControllableEntity::moveDown(float deltaTime)
{
    m_localPosition.x -= m_up.x * m_moveSpeed * deltaTime;
    m_localPosition.y -= m_up.y * m_moveSpeed * deltaTime;
    m_localPosition.z -= m_up.z * m_moveSpeed * deltaTime;
}

// Left-handed rotation follows the direction of the fingers when the thumb points along the rotation axis

void ControllableEntity::rotateLeft(float deltaTime)
{
    m_localEulerAngles.y -= m_rotationSpeed * deltaTime;
    m_localEulerAngles.y = fmod(m_localEulerAngles.y, 360.f);
}

void ControllableEntity::rotateRight(float deltaTime)
{
    m_localEulerAngles.y += m_rotationSpeed * deltaTime;
    m_localEulerAngles.y = fmod(m_localEulerAngles.y, 360.f);
}

void ControllableEntity::rotateUp(float deltaTime)
{
    m_localEulerAngles.x -= m_rotationSpeed * deltaTime;
    if (m_localEulerAngles.x < -90.f)
    {
        m_localEulerAngles.x = -90.f;
    }
}

void ControllableEntity::rotateDown(float deltaTime)
{
    m_localEulerAngles.x += m_rotationSpeed * deltaTime;
    if (m_localEulerAngles.x > 90.f)
    {
        m_localEulerAngles.x = 90.f;
    }
}
