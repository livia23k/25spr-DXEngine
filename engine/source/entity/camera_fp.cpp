#include "entity/camera_fp.h"
#include "entity/entity.h"
#include <algorithm>

FirstPersonCamera::FirstPersonCamera(ID3D11Device *device, EntityBase *targetEntity)
    : CameraBase(device, targetEntity), m_positionOffset(DirectX::XMFLOAT3(0.f, 0.f, 0.f))
{
    updateCameraPosition();
    updateVectorsFromEulerAngles();
    updateViewMatrix();
    updateProjectionMatrix();
}

void FirstPersonCamera::onLogicUpdate(float deltaTime)
{
    updateCameraPosition();
    updateViewMatrix();
}

void FirstPersonCamera::onMouseDrag(float deltaX, float deltaY)
{
    return;
}

void FirstPersonCamera::onZoom(float delta)
{
    return;
}

void FirstPersonCamera::setPositionOffset(const DirectX::XMFLOAT3 &newOffset)
{
    m_position.x = m_position.x - m_positionOffset.x + newOffset.x;
    m_position.y = m_position.y - m_positionOffset.y + newOffset.y;
    m_position.z = m_position.z - m_positionOffset.z + newOffset.z;

    m_positionOffset = newOffset;
    updateViewMatrix();
}

void FirstPersonCamera::updateCameraPosition()
{
    if (!m_targetEntity)
    {
        Logger::LogWarning("FirstPersonCamera::updateCameraPosition: target entity is nullptr");
        return;
    }

    m_front = m_targetEntity->getFrontVector();
    m_up = m_targetEntity->getUpVector();
    m_right = m_targetEntity->getRightVector();

    DirectX::XMFLOAT3 targetPos = m_targetEntity->getWorldPosition();
    DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&targetPos);
    DirectX::XMVECTOR offset = DirectX::XMVectorZero();

    DirectX::XMVECTOR rightOffset = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&m_right), m_positionOffset.x);
    DirectX::XMVECTOR upOffset = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&m_up), m_positionOffset.y);
    DirectX::XMVECTOR frontOffset = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&m_front), m_positionOffset.z);

    offset = DirectX::XMVectorAdd(offset, rightOffset);
    offset = DirectX::XMVectorAdd(offset, upOffset);
    offset = DirectX::XMVectorAdd(offset, frontOffset);

    DirectX::XMVECTOR finalPos = DirectX::XMVectorAdd(posVec, offset);
    DirectX::XMStoreFloat3(&m_position, finalPos);
}
