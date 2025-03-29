#pragma once

#include "entity/entity_controllable.h"
#include "game_forward.h"
#include "celestial_body.h"
#include <memory>
#include <unordered_map>

class Spaceship : public ControllableEntity
{
public:
    enum class State
    {
        Free,
        Landing,
        Orbiting
    };

    struct StateContext
    {
        State state;
        std::shared_ptr<CelestialBody> target;
        std::shared_ptr<Orbit> orbit;

        // transfer
        float transitionProgress;

        // landing
        float landingPhi;
        float landingTheta;
        float landingRadius;
    };

    Spaceship(ID3D11Device *device);
    ~Spaceship() = default;

    DirectX::XMFLOAT3 getWorldPosition() const override;

    void setTransferSpeed(float speed);
    void setState(State newState, std::shared_ptr<CelestialBody> target = nullptr, std::shared_ptr<Orbit> orbit = nullptr);

    void onLogicUpdate(float deltaTime) override;

    void move(float deltaTime, ControllableEntity::MovementDirection direction) override;
    void rotate(float deltaTime, ControllableEntity::RotationDirection direction) override;

private:
    DirectX::XMFLOAT3 calculatePositionForState(float deltaTime, const StateContext &state) const;
    DirectX::XMFLOAT3 calculateRotationForState(float deltaTime, const StateContext &state);

    void updateLandingState(float deltaTime);
    void updateOrbitingState(float deltaTime);
    void updateTransferringState(float deltaTime);

    void startTransition(State newState, std::shared_ptr<CelestialBody> target = nullptr, std::shared_ptr<Orbit> orbit = nullptr);
    void completeTransition();

    void initLandingSettings();
    // void updateLandingSettingsFromCartesian();

    StateContext m_currentState;
    StateContext m_nextState;
    float m_transferSpeed;

    bool m_isTransferring;
    DirectX::XMFLOAT3 m_transitionStartPos;
    DirectX::XMFLOAT3 m_transitionStartRot; // euler angles
};