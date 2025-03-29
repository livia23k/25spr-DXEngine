#pragma once

#include "entity/camera_base.h"

class ThirdPersonCamera : public CameraBase
{
public:
    ThirdPersonCamera(ID3D11Device *device, EntityBase *targetEntity);
    virtual ~ThirdPersonCamera() = default;

    void onLogicUpdate(float deltaTime) override;
    void onMouseDrag(float deltaX, float deltaY) override;
    void onZoom(float delta) override;

    void setDistance(float distance);

protected:
    void updateCameraPosition() override;

private:
    float m_distance;
    float m_zoomSpeed;
};
