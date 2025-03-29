#pragma once

#include "utils/forward.h"

class CameraBase
{
public:
    CameraBase(ID3D11Device *device, EntityBase *targetEntity);
    virtual ~CameraBase() = default;

    void getPosition(DirectX::XMFLOAT3 &position);
    void getViewMatrix(DirectX::XMMATRIX &view);
    void getProjectionMatrix(DirectX::XMMATRIX &projection);
    DirectX::XMFLOAT3 getFrontVector();

    void setFOV(float fov);
    void setAspectRatio(float aspectRatio);
    void setNearFarClip(float nearClip, float farClip);
    void setTargetEntity(EntityBase *entity);

    virtual void onLogicUpdate(float deltaTime) = 0;
    void onGraphicsUpdate(ID3D11DeviceContext *deviceContext);
    virtual void onMouseDrag(float deltaX, float deltaY) = 0;
    virtual void onZoom(float delta) = 0;

protected:
    void updateVectorsFromEulerAngles();
    void updateViewMatrix();
    void updateProjectionMatrix();
    virtual void updateCameraPosition() = 0;

    void initCameraBuffer(ID3D11Device *device);
    void bindCameraBuffer(ID3D11DeviceContext *deviceContext);

    EntityBase *m_targetEntity;

    float m_fov; // in radians
    float m_aspectRatio;
    float m_nearClip;
    float m_farClip;

    DirectX::XMFLOAT3 m_position;

    // Euler angles in degrees (for rotation control)
    float m_yaw;
    float m_pitch;
    // Direction vectors (for view calculation)
    DirectX::XMFLOAT3 m_front;
    DirectX::XMFLOAT3 m_right;
    DirectX::XMFLOAT3 m_up;

    DirectX::XMMATRIX m_viewMatrix;
    DirectX::XMMATRIX m_projectionMatrix;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cameraBuffer;
};
