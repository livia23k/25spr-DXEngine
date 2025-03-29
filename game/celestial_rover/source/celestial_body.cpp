#include "celestial_body.h"

#include <random>

CelestialBody::CelestialBody(ID3D11Device *device)
    : EntityBase(device),
      m_primaryBody(nullptr),
      m_selfRotationSpeed(0.0f),
      m_orbit(nullptr)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 2.0 * DirectX::XM_PI);
    m_selfRotationAngle = static_cast<float>(dis(gen)); // init a random angle
}

DirectX::XMFLOAT3 CelestialBody::getLocalPosition() const
{
    DirectX::XMFLOAT3 localPosition = m_localPosition;
    if (m_orbit)
    {
        DirectX::XMFLOAT3 orbitPosition = m_orbit->getPosition();
        localPosition.x += orbitPosition.x;
        localPosition.y += orbitPosition.y;
        localPosition.z += orbitPosition.z;
    }
    return localPosition;
}

DirectX::XMFLOAT3 CelestialBody::getWorldPosition() const
{
    DirectX::XMFLOAT3 worldPosition = m_localPosition;
    if (m_orbit)
    {
        DirectX::XMFLOAT3 orbitPosition = m_orbit->getPosition();
        worldPosition.x += orbitPosition.x;
        worldPosition.y += orbitPosition.y;
        worldPosition.z += orbitPosition.z;
    }

    if (m_primaryBody)
    {
        DirectX::XMFLOAT3 parentPosition = m_primaryBody->getWorldPosition();
        worldPosition.x += parentPosition.x;
        worldPosition.y += parentPosition.y;
        worldPosition.z += parentPosition.z;
    }
    return worldPosition;
}

DirectX::XMFLOAT3 CelestialBody::getWorldRotation() const
{
    // NOTE: independent of primary body rotation
    DirectX::XMFLOAT3 rotation = m_localEulerAngles;
    rotation.y += DirectX::XMConvertToDegrees(m_selfRotationAngle); // yaw
    return rotation;
}

DirectX::XMFLOAT3 CelestialBody::getWorldScale() const
{
    return m_localScale;
}

DirectX::XMMATRIX CelestialBody::getWorldRotationMatrix() const
{
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
        DirectX::XMConvertToRadians(m_localEulerAngles.x),
        DirectX::XMConvertToRadians(m_localEulerAngles.y) + m_selfRotationAngle,
        DirectX::XMConvertToRadians(m_localEulerAngles.z));
    return rotationMatrix;
}

float CelestialBody::getRadius() const
{
    return m_radius;
}

void CelestialBody::setLocalScale(const DirectX::XMFLOAT3 &scale)
{
    if (scale.x != scale.y || scale.x != scale.z)
    {
        Logger::LogError("CelestialBody::setLocalScale: scale in 3D must be equal");
        return;
    }

    m_localScale = scale;
    m_radius = scale.x;
}

void CelestialBody::setOrbit(std::shared_ptr<Orbit> orbit) { m_orbit = orbit; }
void CelestialBody::setPrimaryBody(std::shared_ptr<CelestialBody> primaryBody) { m_primaryBody = primaryBody; }
void CelestialBody::setSelfRotationSpeed(float rotationSpeed) { m_selfRotationSpeed = rotationSpeed; }

void CelestialBody::onLogicUpdate(float deltaTime)
{
    if (m_orbit)
    {
        m_orbit->update(deltaTime);
    }

    m_selfRotationAngle -= m_selfRotationSpeed * deltaTime; // CCW
    m_selfRotationAngle = fmod(m_selfRotationAngle, DirectX::XM_2PI);

    updateWorldMatrix();
    updateDirectionVectors();
}

void CelestialBody::updateWorldMatrix()
{
    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_localScale.x, m_localScale.y, m_localScale.z);
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
        DirectX::XMConvertToRadians(m_localEulerAngles.x),
        DirectX::XMConvertToRadians(m_localEulerAngles.y) + m_selfRotationAngle,
        DirectX::XMConvertToRadians(m_localEulerAngles.z));
    DirectX::XMFLOAT3 worldPosition = getWorldPosition();
    DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(worldPosition.x, worldPosition.y, worldPosition.z);

    m_worldMatrix = scaleMatrix * rotationMatrix * translationMatrix; // S * R * T (S first)
}
