#pragma once

#include "utils/forward.h"
#include "game_forward.h"
#include "resources/render_component.h"

#include <vector>
#include <atomic>

class EntityBase
{
public:
    EntityBase(ID3D11Device *device);
    virtual ~EntityBase() = default;

    uint32_t getId() const;
    std::shared_ptr<EntityBase> getParent() const;
    std::vector<std::shared_ptr<EntityBase>> getChildren() const;
    DirectX::XMFLOAT3 getUpVector() const;
    DirectX::XMFLOAT3 getRightVector() const;
    DirectX::XMFLOAT3 getFrontVector() const;
    virtual DirectX::XMFLOAT3 getWorldPosition() const;
    virtual DirectX::XMFLOAT3 getWorldRotation() const;
    virtual DirectX::XMFLOAT3 getWorldScale() const;
    virtual DirectX::XMMATRIX getWorldMatrix() const;
    std::vector<std::shared_ptr<RenderComponent>> getRenderComponents() const;

    void setParent(std::shared_ptr<EntityBase> parent);
    void addChild(std::shared_ptr<EntityBase> child);
    void setLocalPosition(const DirectX::XMFLOAT3 &position);
    void setLocalRotation(const DirectX::XMFLOAT3 &eulerAngles);
    virtual void setLocalScale(const DirectX::XMFLOAT3 &scale);

    virtual void onLogicUpdate(float deltaTime);
    void onGraphicsUpdate(ID3D11DeviceContext *deviceContext);

    void addRenderComponent(std::shared_ptr<RenderComponent> renderComponent);

protected:
    virtual void updateWorldMatrix();
    void updateDirectionVectors();

    void initModelBuffer(ID3D11Device *device);
    void bindModelBuffer(ID3D11DeviceContext *deviceContext);

    static std::atomic<uint32_t> s_nextId;

    uint32_t m_id;
    std::shared_ptr<EntityBase> m_parent;
    std::vector<std::shared_ptr<EntityBase>> m_children;

    DirectX::XMFLOAT3 m_localPosition;
    DirectX::XMFLOAT3 m_localEulerAngles; // pitch, yaw, roll (in degrees)
    DirectX::XMFLOAT3 m_localScale;

    DirectX::XMMATRIX m_worldMatrix;

    DirectX::XMFLOAT3 m_front;
    DirectX::XMFLOAT3 m_right;
    DirectX::XMFLOAT3 m_up;

    std::vector<std::shared_ptr<RenderComponent>> m_renderComponents;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_modelBuffer;
};
