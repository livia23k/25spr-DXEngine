#include "orbit.h"
#include <random>

Orbit::Orbit(DirectX::XMFLOAT3 center, float semiMajorAxis, float eccentricity, float angularSpeed)
    : m_center(center),
      m_semiMajorAxis(semiMajorAxis),
      m_eccentricity(eccentricity),
      m_angularSpeed(angularSpeed)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 2.0 * DirectX::XM_PI);
    m_orbitAngle = static_cast<float>(dis(gen)); // init a random angle
}

void Orbit::update(float deltaTime)
{
    m_orbitAngle -= m_angularSpeed * deltaTime;
    m_orbitAngle = fmodf(m_orbitAngle, 2.0 * DirectX::XM_PI);
}

DirectX::XMFLOAT3 Orbit::getPosition() const
{
    float reverseOrbitAngle = -m_orbitAngle; // increasing angle corresponds to CCW in polar coordinates
    float semiMinorAxis = m_semiMajorAxis * sqrt(1.f - m_eccentricity * m_eccentricity);

    float x = m_center.x + m_semiMajorAxis * cos(reverseOrbitAngle);
    float y = m_center.y;
    float z = m_center.z + semiMinorAxis * sin(reverseOrbitAngle);
    return DirectX::XMFLOAT3(x, y, z);
}

DirectX::XMFLOAT3 Orbit::getRotation() const
{
    float yaw = DirectX::XMConvertToDegrees(m_orbitAngle);
    float pitch = 0.f;
    float roll = 0.f;
    return DirectX::XMFLOAT3(pitch, yaw, roll);
}
