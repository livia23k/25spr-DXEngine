#pragma once

#include "utils/forward.h"
#include "entity/entity.h"
#include "game_forward.h"
#include "orbit.h"

class CelestialBody : public EntityBase
{
public:
    CelestialBody(ID3D11Device *device);

    DirectX::XMFLOAT3 getLocalPosition() const;
    DirectX::XMFLOAT3 getWorldPosition() const override;
    DirectX::XMFLOAT3 getWorldRotation() const override;
    DirectX::XMFLOAT3 getWorldScale() const override;
    DirectX::XMMATRIX getWorldRotationMatrix() const;
    float getRadius() const;

    void setLocalScale(const DirectX::XMFLOAT3 &scale) override;
    void setOrbit(std::shared_ptr<Orbit> orbit);
    void setPrimaryBody(std::shared_ptr<CelestialBody> primaryBody);
    void setSelfRotationSpeed(float rotationSpeed);

    void onLogicUpdate(float deltaTime) override;

protected:
    void updateWorldMatrix() override;

    float m_radius; // ratio based on r=1.f sphere

    // std::shared_ptr<CelestialBody> m_parent; // WARNING: do not use m_parent in this class
    std::shared_ptr<CelestialBody> m_primaryBody;

    std::shared_ptr<Orbit> m_orbit;
    float m_selfRotationSpeed; // rad/s
    float m_selfRotationAngle; // rad, [0, 2pi)
};
