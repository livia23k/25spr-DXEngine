#include "entity/camera_tp.h"
#include "entity/entity.h"
#include <algorithm>

ThirdPersonCamera::ThirdPersonCamera(ID3D11Device *device, EntityBase *targetEntity)
    : CameraBase(device, targetEntity), m_distance(10.0f), m_zoomSpeed(1.0f)
{
    updateVectorsFromEulerAngles();
    updateCameraPosition();
    updateViewMatrix();
    updateProjectionMatrix();
}

void ThirdPersonCamera::onLogicUpdate(float deltaTime)
{
    updateCameraPosition();
    updateViewMatrix();
}

void ThirdPersonCamera::onMouseDrag(float deltaX, float deltaY)
{
    m_yaw += deltaX;
    m_pitch += deltaY;
    updateVectorsFromEulerAngles();
    updateViewMatrix();
}

void ThirdPersonCamera::onZoom(float delta)
{
    this->setDistance(m_distance - delta * m_zoomSpeed);
}

void ThirdPersonCamera::setDistance(float distance)
{
    m_distance = distance;
    m_distance = std::clamp(m_distance, 10.0f, 100.0f);
    updateCameraPosition();
    updateViewMatrix();
}

void ThirdPersonCamera::updateCameraPosition()
{
    if (!m_targetEntity)
    {
        Logger::LogWarning("ThirdPersonCamera::updateCameraPosition: target entity is nullptr");
        return;
    }

    DirectX::XMFLOAT3 targetPosition = m_targetEntity->getWorldPosition();
    m_position.x = targetPosition.x - m_front.x * m_distance;
    m_position.y = targetPosition.y - m_front.y * m_distance;
    m_position.z = targetPosition.z - m_front.z * m_distance;
}
