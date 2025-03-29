#pragma once

#include "entity/camera_base.h"

class FirstPersonCamera : public CameraBase
{
public:
    FirstPersonCamera(ID3D11Device *device, EntityBase *targetEntity);
    virtual ~FirstPersonCamera() = default;

    void onLogicUpdate(float deltaTime) override;
    void onMouseDrag(float deltaX, float deltaY) override;
    void onZoom(float delta) override;

    void setPositionOffset(const DirectX::XMFLOAT3 &newOffset);

protected:
    void updateCameraPosition() override;

private:
    DirectX::XMFLOAT3 m_positionOffset; // right, up, forward
};
