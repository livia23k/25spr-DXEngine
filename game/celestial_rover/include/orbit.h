#pragma once

#include "utils/forward.h"

// Orbit on xz-plane
class Orbit
{
public:
    Orbit(DirectX::XMFLOAT3 center, float semiMajorAxis, float eccentricity, float angularSpeed);

    void update(float deltaTime);

    DirectX::XMFLOAT3 getPosition() const;
    DirectX::XMFLOAT3 getRotation() const;

private:
    DirectX::XMFLOAT3 m_center;
    float m_semiMajorAxis;
    float m_eccentricity; // 0 = circle, [0, 1) = ellipse, 1 = parabola, > 1 = hyperbola
    float m_angularSpeed; // rad/s
    float m_orbitAngle;   // rad, [0, 2pi)
};
