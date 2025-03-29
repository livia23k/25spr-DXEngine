#include "entity/camera_base.h"
#include "entity/entity.h"
#include "resources/buffer_type.h"
#include <algorithm>

CameraBase::CameraBase(ID3D11Device *device, EntityBase *targetEntity)
    : m_targetEntity(targetEntity), m_fov(DirectX::XM_PIDIV4), // 45 degrees in radians
      m_aspectRatio(16.0f / 9.0f), m_nearClip(0.1f), m_position(0.0f, 0.0f, 0.0f), m_farClip(10000.0f), m_yaw(0.0f), m_pitch(0.0f)
{
    updateVectorsFromEulerAngles();
    // updateCameraPosition(); // need to be called in child constructor
    updateViewMatrix();
    updateProjectionMatrix();
    initCameraBuffer(device);
}

void CameraBase::getPosition(DirectX::XMFLOAT3 &position) { position = m_position; }
void CameraBase::getViewMatrix(DirectX::XMMATRIX &view) { view = m_viewMatrix; }
void CameraBase::getProjectionMatrix(DirectX::XMMATRIX &projection) { projection = m_projectionMatrix; }
DirectX::XMFLOAT3 CameraBase::getFrontVector() { return m_front; }

void CameraBase::setFOV(float fov)
{
    m_fov = fov;
    updateProjectionMatrix();
}

void CameraBase::setAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
    updateProjectionMatrix();
}

void CameraBase::setNearFarClip(float nearClip, float farClip)
{
    m_nearClip = nearClip;
    m_farClip = farClip;
    updateProjectionMatrix();
}

void CameraBase::setTargetEntity(EntityBase *entity)
{
    if (entity == nullptr)
    {
        Logger::LogWarning("CameraBase::setTargetEntity: entity is nullptr");
        return;
    }

    if (entity == m_targetEntity)
    {
        Logger::LogWarning("CameraBase::setTargetEntity: entity is the same as the current target entity");
        return;
    }

    m_targetEntity = entity;
    m_yaw = 0.0f;
    m_pitch = 0.0f;
    updateVectorsFromEulerAngles();
    updateCameraPosition();
    updateViewMatrix();
}

void CameraBase::onGraphicsUpdate(ID3D11DeviceContext *deviceContext)
{
    bindCameraBuffer(deviceContext);
}

void CameraBase::updateVectorsFromEulerAngles()
{
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    m_yaw = std::fmod(m_yaw + 180.0f, 360.0f) - 180.0f;

    float yawRad = DirectX::XMConvertToRadians(m_yaw);
    float pitchRad = DirectX::XMConvertToRadians(m_pitch);

    float cosPitch = cosf(pitchRad);
    float sinPitch = sinf(pitchRad);
    float cosYaw = cosf(yawRad);
    float sinYaw = sinf(yawRad);

    m_front.x = sinYaw * cosPitch; // +X right
    m_front.y = sinPitch;          // +Y up
    m_front.z = cosYaw * cosPitch; // +Z forward

    DirectX::XMVECTOR frontVec = DirectX::XMLoadFloat3(&m_front);
    frontVec = DirectX::XMVector3Normalize(frontVec);
    DirectX::XMStoreFloat3(&m_front, frontVec);

    DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR rightVec = DirectX::XMVector3Cross(upVec, frontVec);
    rightVec = DirectX::XMVector3Normalize(rightVec);
    DirectX::XMStoreFloat3(&m_right, rightVec);

    DirectX::XMVECTOR newUpVec = DirectX::XMVector3Cross(frontVec, rightVec);
    newUpVec = DirectX::XMVector3Normalize(newUpVec);
    DirectX::XMStoreFloat3(&m_up, newUpVec);
}

void CameraBase::updateViewMatrix()
{
    DirectX::XMFLOAT3 target;
    target.x = m_position.x + m_front.x;
    target.y = m_position.y + m_front.y;
    target.z = m_position.z + m_front.z;

    m_viewMatrix = DirectX::XMMatrixLookAtLH(
        DirectX::XMLoadFloat3(&m_position),
        DirectX::XMLoadFloat3(&target),
        DirectX::XMLoadFloat3(&m_up));
}

void CameraBase::updateProjectionMatrix()
{
    m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_fov, m_aspectRatio, m_nearClip, m_farClip);
}

void CameraBase::initCameraBuffer(ID3D11Device *device)
{
    if (!device)
    {
        throw std::runtime_error("CameraBase::initCameraBuffer: device is nullptr");
    }

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(CameraBuffer);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    CameraBuffer camBuffer = {};
    camBuffer.view = DirectX::XMMatrixTranspose(m_viewMatrix);
    camBuffer.projection = DirectX::XMMatrixTranspose(m_projectionMatrix);

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &camBuffer;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, m_cameraBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error(std::format("CameraBase::initCameraBuffer: failed to create camera buffer, HRESULT = {:#X}", hr));
    }
}

void CameraBase::bindCameraBuffer(ID3D11DeviceContext *deviceContext)
{
    if (!deviceContext)
    {
        throw std::runtime_error("CameraBase::bindCameraBuffer: deviceContext is nullptr");
    }

    CameraBuffer camBuffer;
    camBuffer.view = DirectX::XMMatrixTranspose(m_viewMatrix);
    camBuffer.projection = DirectX::XMMatrixTranspose(m_projectionMatrix);

    deviceContext->UpdateSubresource(m_cameraBuffer.Get(), 0, nullptr, &camBuffer, 0, 0);
    deviceContext->VSSetConstantBuffers(1, 1, m_cameraBuffer.GetAddressOf()); // slot 1
}
