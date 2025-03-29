#include "resources/game_resource_mgr.h"
#include "resources/render_component.h"
#include "graphics/dx11/dx_device_mgr.h"
#include "resources/buffer_type.h"

GameResourceManager::GameResourceManager(ID3D11Device *device) : m_device(device) {}
GameResourceManager::~GameResourceManager() {}

void GameResourceManager::registerStaticEntity(std::shared_ptr<EntityBase> entity)
{
    if (entity == nullptr)
    {
        Logger::LogWarning("GameResourceManager::registerStaticEntity: entity is nullptr");
        return;
    };
    m_allEntities[entity->getId()] = entity;
}

void GameResourceManager::registerControllableEntity(std::shared_ptr<ControllableEntity> entity)
{
    if (entity == nullptr)
    {
        Logger::LogWarning("GameResourceManager::registerControllableEntity: entity is nullptr");
        return;
    }

    uint32_t entityId = entity->getId();
    m_allEntities[entityId] = entity;
    m_controllableEntities[entityId] = entity;
}

void GameResourceManager::registerLight(std::shared_ptr<Light> light)
{
    if (light == nullptr)
    {
        Logger::LogWarning("GameResourceManager::registerLight: light is nullptr");
        return;
    }

    uint32_t lightId = light->getId();
    m_lights[lightId] = light;
}

std::shared_ptr<EntityBase> GameResourceManager::getEntity(uint32_t id) const
{
    auto it = m_allEntities.find(id);
    return (it != m_allEntities.end()) ? it->second : nullptr;
}

std::shared_ptr<ControllableEntity> GameResourceManager::getControllableEntity(uint32_t id) const
{
    auto it = m_controllableEntities.find(id);
    return (it != m_controllableEntities.end()) ? it->second : nullptr;
}

std::shared_ptr<Light> GameResourceManager::getLight(uint32_t id) const
{
    auto it = m_lights.find(id);
    return (it != m_lights.end()) ? it->second : nullptr;
}

void GameResourceManager::onLogicUpdate(float deltaTime)
{
    for (auto &pair : m_rootEntities)
    {
        topDownLogicUpdateRecursive(pair.second, deltaTime);
    }
}

void GameResourceManager::onGraphicsUpdate(DXDeviceManager *deviceManager)
{
    auto deviceContext = deviceManager->getDeviceContext();

    bindLightArrayBuffer(deviceContext);

    for (auto &pair : m_allEntities)
    {
        std::shared_ptr<EntityBase> entity = pair.second;
        entity->onGraphicsUpdate(deviceContext); // bind model buffer
        for (auto &comp : entity->getRenderComponents())
        {
            if (comp->getIsCullFront())
            {
                deviceManager->setRasterStateCullFront();
            }
            else
            {
                deviceManager->setRasterStateCullBack();
            }

            comp->render(deviceContext); // bind render comp (mesh, material, shader, etc.)
        }
    }
}

void GameResourceManager::rebuildRootEntities()
{
    m_rootEntities.clear();
    for (auto &pair : m_allEntities)
    {
        std::shared_ptr<EntityBase> entity = pair.second;
        if (entity->getParent() == nullptr)
        {
            m_rootEntities[entity->getId()] = entity;
        }
    }
}

void GameResourceManager::topDownLogicUpdateRecursive(std::shared_ptr<EntityBase> entity, float deltaTime)
{
    entity->onLogicUpdate(deltaTime);
    std::vector<std::shared_ptr<EntityBase>> children = entity->getChildren();
    for (auto &child : children)
    {
        topDownLogicUpdateRecursive(child, deltaTime);
    }
}

void GameResourceManager::initLightArrayBuffer(ID3D11Device *device)
{
    if (!device)
    {
        throw std::runtime_error("GameResourceManager::initLightArrayBuffer: device is nullptr");
    }

    if (m_lights.empty())
    {
        return;
    }

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(LightArrayBuffer);
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    LightArrayBuffer lightArrayBuffer = {};
    lightArrayBuffer.numLights = min(MAX_LIGHTS, (int)m_lights.size());
    for (int i = 0; i < lightArrayBuffer.numLights; i++)
    {
        lightArrayBuffer.lights[i] = m_lights[i]->getLightBuffer();
    }

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &lightArrayBuffer;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, m_lightArrayBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("GameResourceManager::initLightArrayBuffer: failed to create light array buffer");
    }
}

void GameResourceManager::bindLightArrayBuffer(ID3D11DeviceContext *context)
{
    if (!context)
    {
        throw std::runtime_error("GameResourceManager::updateLightArrayBuffer: context is nullptr");
    }

    if (m_lights.empty())
    {
        return;
    }

    LightArrayBuffer lightArrayBuffer = {};
    lightArrayBuffer.numLights = min(MAX_LIGHTS, (int)m_lights.size());
    for (int i = 0; i < lightArrayBuffer.numLights; i++)
    {
        lightArrayBuffer.lights[i] = m_lights[i]->getLightBuffer();
    }

    context->UpdateSubresource(m_lightArrayBuffer.Get(), 0, nullptr, &lightArrayBuffer, 0, 0);
    context->PSSetConstantBuffers(3, 1, m_lightArrayBuffer.GetAddressOf()); // slot 3
}
