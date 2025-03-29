
#pragma once

#include "entity/entity.h"

class ControllableEntity : public EntityBase
{
public:
    enum class MovementDirection
    {
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Down
    };
    enum class RotationDirection
    {
        YawLeft,
        YawRight,
        PitchUp,
        PitchDown
    };

    ControllableEntity(ID3D11Device *device);
    virtual ~ControllableEntity() = default;

    void getMoveSpeed(float &moveSpeed) const;
    void getRotationSpeed(float &rotationSpeed) const;

    void setMoveSpeed(float moveSpeed);
    void setRotationSpeed(float rotationSpeed);

    virtual void move(float deltaTime, MovementDirection direction);
    virtual void rotate(float deltaTime, RotationDirection direction);

protected:
    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void moveLeft(float deltaTime);
    void moveRight(float deltaTime);
    void moveUp(float deltaTime);
    void moveDown(float deltaTime);

    void rotateLeft(float deltaTime);
    void rotateRight(float deltaTime);
    void rotateUp(float deltaTime);
    void rotateDown(float deltaTime);

    bool m_isUnderControl;
    float m_moveSpeed;
    float m_rotationSpeed;
};