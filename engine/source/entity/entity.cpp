#include "entity/entity.h"
#include "resources/buffer_type.h"

std::atomic<uint32_t> EntityBase::s_nextId = 0;

EntityBase::EntityBase(ID3D11Device *device)
    : m_id(s_nextId.fetch_add(1)),
      m_localPosition({0.0f, 0.0f, 0.0f}),
      m_localEulerAngles({0.0f, 0.0f, 0.0f}),
      m_localScale({1.0f, 1.0f, 1.0f}),
      m_parent(nullptr)
{
    updateWorldMatrix();
    updateDirectionVectors();
    initModelBuffer(device);
}

uint32_t EntityBase::getId() const { return m_id; }
std::shared_ptr<EntityBase> EntityBase::getParent() const { return m_parent; }
std::vector<std::shared_ptr<EntityBase>> EntityBase::getChildren() const { return m_children; }
DirectX::XMFLOAT3 EntityBase::getUpVector() const { return m_up; }
DirectX::XMFLOAT3 EntityBase::getRightVector() const { return m_right; }
DirectX::XMFLOAT3 EntityBase::getFrontVector() const { return m_front; }

DirectX::XMFLOAT3 EntityBase::getWorldPosition() const
{
    DirectX::XMFLOAT3 worldPosition = m_localPosition;
    std::shared_ptr<EntityBase> currentParent = m_parent;
    while (currentParent)
    {
        worldPosition.x += currentParent->m_localPosition.x;
        worldPosition.y += currentParent->m_localPosition.y;
        worldPosition.z += currentParent->m_localPosition.z;
        currentParent = currentParent->m_parent;
    }
    return worldPosition;
}

DirectX::XMFLOAT3 EntityBase::getWorldRotation() const
{
    DirectX::XMFLOAT3 worldRotation = m_localEulerAngles;
    std::shared_ptr<EntityBase> currentParent = m_parent;
    while (currentParent)
    {
        worldRotation.x += currentParent->m_localEulerAngles.x;
        worldRotation.y += currentParent->m_localEulerAngles.y;
        worldRotation.z += currentParent->m_localEulerAngles.z;
        currentParent = currentParent->m_parent;
    }
    return DirectX::XMFLOAT3(
        fmod(worldRotation.x + 180.f, 360.f) - 180.f,
        fmod(worldRotation.y + 180.f, 360.f) - 180.f,
        fmod(worldRotation.z + 180.f, 360.f) - 180.f);
}

DirectX::XMFLOAT3 EntityBase::getWorldScale() const
{
    DirectX::XMFLOAT3 worldScale = m_localScale;
    std::shared_ptr<EntityBase> currentParent = m_parent;
    while (currentParent)
    {
        worldScale.x *= currentParent->m_localScale.x;
        worldScale.y *= currentParent->m_localScale.y;
        worldScale.z *= currentParent->m_localScale.z;
        currentParent = currentParent->m_parent;
    }
    return worldScale;
}

DirectX::XMMATRIX EntityBase::getWorldMatrix() const { return m_worldMatrix; }
std::vector<std::shared_ptr<RenderComponent>> EntityBase::getRenderComponents() const { return m_renderComponents; }

void EntityBase::setParent(std::shared_ptr<EntityBase> parent) { m_parent = parent; }
void EntityBase::addChild(std::shared_ptr<EntityBase> child) { m_children.push_back(child); }
void EntityBase::setLocalPosition(const DirectX::XMFLOAT3 &position) { m_localPosition = position; }

void EntityBase::setLocalRotation(const DirectX::XMFLOAT3 &eulerAngles)
{
    m_localEulerAngles = eulerAngles;
    updateWorldMatrix();
    updateDirectionVectors();
}

void EntityBase::setLocalScale(const DirectX::XMFLOAT3 &scale) { m_localScale = scale; }

void EntityBase::onLogicUpdate(float deltaTime)
{
    updateWorldMatrix();
    updateDirectionVectors();
}

void EntityBase::onGraphicsUpdate(ID3D11DeviceContext *deviceContext)
{
    bindModelBuffer(deviceContext);
}

void EntityBase::addRenderComponent(std::shared_ptr<RenderComponent> renderComponent)
{
    m_renderComponents.push_back(renderComponent);
}

void EntityBase::updateDirectionVectors()
{
    // FLOAT4X4 is row-major, while XMMATRIX is column-major
    // In row-major, the relationship between the world matrix and the direction vectors is:
    // [Right.xyz, 0.f]
    // [Up.xyz, 0.f]
    // [Front.xyz, 0.f]
    // [Position.xyz, 1.f]

    DirectX::XMFLOAT4X4 worldMatrix;
    DirectX::XMStoreFloat4x4(&worldMatrix, m_worldMatrix);

    m_right.x = worldMatrix._11;
    m_right.y = worldMatrix._12;
    m_right.z = worldMatrix._13;

    m_up.x = worldMatrix._21;
    m_up.y = worldMatrix._22;
    m_up.z = worldMatrix._23;

    m_front.x = worldMatrix._31;
    m_front.y = worldMatrix._32;
    m_front.z = worldMatrix._33;
}

void EntityBase::updateWorldMatrix()
{
    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_localScale.x, m_localScale.y, m_localScale.z);
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
        DirectX::XMConvertToRadians(m_localEulerAngles.x),
        DirectX::XMConvertToRadians(m_localEulerAngles.y),
        DirectX::XMConvertToRadians(m_localEulerAngles.z));
    DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_localPosition.x, m_localPosition.y, m_localPosition.z);

    if (m_parent)
    {
        m_worldMatrix = scaleMatrix * rotationMatrix * translationMatrix * m_parent->getWorldMatrix(); // S * R * T * P (S first) // WARNING: ensure updating parents matrix before children
    }
    else
    {
        m_worldMatrix = scaleMatrix * rotationMatrix * translationMatrix; // S * R * T (S first)
    }
}

void EntityBase::initModelBuffer(ID3D11Device *device)
{
    if (!device)
    {
        throw std::runtime_error("Entity::initModelBuffer: device is nullptr");
    }

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(ModelBuffer);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    ModelBuffer modelBuffer = {};
    modelBuffer.world = XMMatrixTranspose(m_worldMatrix);

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &modelBuffer;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, m_modelBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error(std::format("Entity::initModelBuffer: failed to create model buffer, HRESULT: {:#X}", hr));
    }
}

void EntityBase::bindModelBuffer(ID3D11DeviceContext *deviceContext)
{
    ModelBuffer modelBuffer;
    modelBuffer.world = DirectX::XMMatrixTranspose(m_worldMatrix);

    deviceContext->UpdateSubresource(m_modelBuffer.Get(), 0, nullptr, &modelBuffer, 0, 0);
    deviceContext->VSSetConstantBuffers(0, 1, m_modelBuffer.GetAddressOf()); // slot 0
}
