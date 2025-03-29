#include "spaceship.h"
#include <cmath>
#include <algorithm>

Spaceship::Spaceship(ID3D11Device *device)
    : ControllableEntity(device), m_currentState{State::Free, nullptr, nullptr, 1.0f}, m_nextState{State::Free, nullptr, nullptr, 0.0f}, m_transferSpeed(0.1f), m_isTransferring(false)
{
    m_isUnderControl = true;
}

namespace {
    DirectX::XMFLOAT3 lerpFloat3(const DirectX::XMFLOAT3 &a, const DirectX::XMFLOAT3 &b, float t)
    {
        DirectX::XMFLOAT3 result;
        result.x = a.x + (b.x - a.x) * t;
        result.y = a.y + (b.y - a.y) * t;
        result.z = a.z + (b.z - a.z) * t;
        return result;
    }

    float smoothStep(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    DirectX::XMFLOAT3 sphericalToCartesian(const float &radius, const float &theta, const float &phi)
    {
        DirectX::XMFLOAT3 result;
        result.x = radius * sin(theta) * cos(phi);
        result.y = radius * cos(theta);
        result.z = radius * sin(theta) * sin(phi);
        return result;
    }
}


DirectX::XMFLOAT3 Spaceship::getWorldPosition() const { return m_localPosition; }

void Spaceship::setTransferSpeed(float speed) { m_transferSpeed = speed; }

void Spaceship::setState(State newState, std::shared_ptr<CelestialBody> target, std::shared_ptr<Orbit> orbit)
{
    if (m_isTransferring)
    {
        return;
    }

    if (newState == m_currentState.state && target == m_currentState.target && orbit == m_currentState.orbit)
    {
        return;
    }

    std::string stateStr = newState == State::Free ? "Free" : newState == State::Landing ? "Landing"
                                                          : newState == State::Orbiting  ? "Orbiting"
                                                                                         : "Unknown";
    Logger::LogInfo("Spaceship::setState: Setting state to " + stateStr);

    startTransition(newState, target, orbit);
}

void Spaceship::onLogicUpdate(float deltaTime)
{
    if (m_isTransferring)
    {
        updateTransferringState(deltaTime);
    }
    else
    {
        switch (m_currentState.state)
        {
        case State::Free:
            break;
        case State::Landing:
            updateLandingState(deltaTime);
            break;
        case State::Orbiting:
            if (m_currentState.orbit)
            {
                m_currentState.orbit->update(deltaTime);
            }
            updateOrbitingState(deltaTime);
            break;
        default:
            Logger::LogError("Spaceship::onLogicUpdate: Unknown state");
            break;
        }
    }

    updateWorldMatrix();
    updateDirectionVectors();
}

void Spaceship::updateLandingState(float deltaTime)
{
    m_localPosition = calculatePositionForState(deltaTime, m_currentState);
    m_localEulerAngles = calculateRotationForState(deltaTime, m_currentState);
}

void Spaceship::updateOrbitingState(float deltaTime)
{
    m_localPosition = calculatePositionForState(deltaTime, m_currentState);
    m_localEulerAngles = calculateRotationForState(deltaTime, m_currentState);
}

void Spaceship::updateTransferringState(float deltaTime)
{
    m_nextState.transitionProgress += deltaTime * m_transferSpeed;
    // Logger::LogInfo(std::format("Transition progress: {}", m_nextState.transitionProgress));

    DirectX::XMFLOAT3 targetPos = calculatePositionForState(deltaTime, m_nextState);
    float smoothT = smoothStep(m_nextState.transitionProgress);
    m_localPosition = lerpFloat3(m_transitionStartPos, targetPos, smoothT);

    DirectX::XMFLOAT3 targetRot = calculateRotationForState(deltaTime, m_nextState);
    m_localEulerAngles = lerpFloat3(m_transitionStartRot, targetRot, smoothT);

    if (m_nextState.transitionProgress >= 1.0f)
    {
        completeTransition();
    }
}

void Spaceship::move(float deltaTime, ControllableEntity::MovementDirection direction)
{
    if (m_currentState.state == State::Free)
    {
        ControllableEntity::move(deltaTime, direction);
    }
    else if (m_currentState.state == State::Landing)
    {
        const float angularSpeed = 0.5f * deltaTime;

        switch (direction)
        {
        case MovementDirection::Forward:
            m_currentState.landingPhi += angularSpeed;
            if (m_currentState.landingPhi > DirectX::XM_PI)
                m_currentState.landingPhi -= DirectX::XM_2PI;
            break;
        case MovementDirection::Backward:
            m_currentState.landingPhi -= angularSpeed;
            if (m_currentState.landingPhi < -DirectX::XM_PI)
                m_currentState.landingPhi += DirectX::XM_2PI;
            break;
        case MovementDirection::Left:
            m_currentState.landingTheta = std::clamp(m_currentState.landingTheta - angularSpeed, 0.01f, DirectX::XM_PI - 0.01f);
            break;
        case MovementDirection::Right:
            m_currentState.landingTheta = std::clamp(m_currentState.landingTheta + angularSpeed, 0.01f, DirectX::XM_PI - 0.01f);
            break;
        default:
            break; // Up/Down ignored in landing
        }
    }
}

void Spaceship::rotate(float deltaTime, ControllableEntity::RotationDirection direction)
{
    if (m_currentState.state == State::Free)
    {
        ControllableEntity::rotate(deltaTime, direction);
    }
}

DirectX::XMFLOAT3 Spaceship::calculatePositionForState(float deltaTime, const StateContext &state) const
{
    switch (state.state)
    {
    case State::Free:
        return m_localPosition;
    case State::Landing:
        if (state.target)
        {
            DirectX::XMFLOAT3 targetPos = state.target->getWorldPosition();
            DirectX::XMMATRIX targetRotMatrix = state.target->getWorldRotationMatrix();

            // make spaceship following the target rotation
            DirectX::XMFLOAT3 landingOffset = sphericalToCartesian(state.landingRadius, state.landingTheta, state.landingPhi);
            DirectX::XMVECTOR landingOffsetVector = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&landingOffset), targetRotMatrix);
            DirectX::XMStoreFloat3(&landingOffset, landingOffsetVector);

            return DirectX::XMFLOAT3(
                targetPos.x + landingOffset.x,
                targetPos.y + landingOffset.y,
                targetPos.z + landingOffset.z);
        }
        return m_localPosition;
    case State::Orbiting:
        if (state.target && state.orbit)
        {
            DirectX::XMFLOAT3 orbitPos = state.orbit->getPosition();
            DirectX::XMFLOAT3 targetPos = state.target->getWorldPosition();
            return {targetPos.x + orbitPos.x, targetPos.y + orbitPos.y, targetPos.z + orbitPos.z};
        }
        return m_localPosition;
    default:
        return m_localPosition;
    }
}

DirectX::XMFLOAT3 Spaceship::calculateRotationForState(float deltaTime, const StateContext &state)
{
    switch (state.state)
    {
    case State::Free:
        return m_localEulerAngles;
    case State::Landing:
    {
        DirectX::XMFLOAT3 targetRot = state.target->getWorldRotation();

        float pitch = 0.f;
        float yaw = targetRot.y + DirectX::XMConvertToDegrees(-state.landingPhi);
        float roll = 90.f * sinf(-state.landingTheta); // 90 at equator, 0 at poles // TODO: optimize orbit from equator to south pole

        return DirectX::XMFLOAT3(pitch, yaw, roll);
    }
    case State::Orbiting:
    {
        if (state.target && state.orbit)
        {
            return state.orbit->getRotation();
        }
        return m_localEulerAngles;
    }
    default:
    {
        return m_localEulerAngles;
    }
    }
}

void Spaceship::startTransition(State newState, std::shared_ptr<CelestialBody> target, std::shared_ptr<Orbit> orbit)
{
    m_isTransferring = true;
    m_transitionStartPos = m_localPosition;
    m_transitionStartRot = m_localEulerAngles;
    m_nextState = {newState, target, orbit, 0.0f, 0.0f, 0.0f, 0.0f};

    if (newState == State::Landing)
    {
        initLandingSettings();
    }
}

void Spaceship::completeTransition()
{
    m_isTransferring = false;
    m_currentState = m_nextState;
    m_currentState.transitionProgress = 1.0f;

    m_nextState = {State::Free, nullptr, nullptr, 0.0f, 0.0f, 0.0f, 0.0f};
}

void Spaceship::initLandingSettings()
{
    DirectX::XMFLOAT3 targetPos = m_nextState.target->getWorldPosition();
    float relativeX = m_localPosition.x - targetPos.x;
    float relativeY = m_localPosition.y - targetPos.y;
    float relativeZ = m_localPosition.z - targetPos.z;

    float radius = sqrt(relativeX * relativeX + relativeY * relativeY + relativeZ * relativeZ);
    float theta = acos(relativeY / radius);
    float phi = atan2(relativeZ, relativeX); // TODO: check

    m_nextState.landingPhi = phi;
    m_nextState.landingTheta = theta;
    m_nextState.landingRadius = m_nextState.target->getRadius() + m_localScale.y; // suppose the original model ranging [-1, 1] in y direction
}
