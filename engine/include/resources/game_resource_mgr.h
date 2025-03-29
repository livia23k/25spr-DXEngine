#pragma once
#include "utils/forward.h"
#include "entity/entity.h"
#include "entity/entity_controllable.h"
#include "entity/light.h"

#include <unordered_map>

class GameResourceManager
{
public:
    GameResourceManager(ID3D11Device *device);
    ~GameResourceManager();

    std::shared_ptr<EntityBase> getEntity(uint32_t id) const;
    std::shared_ptr<ControllableEntity> getControllableEntity(uint32_t id) const;
    std::shared_ptr<Light> getLight(uint32_t id) const;
    
    void registerStaticEntity(std::shared_ptr<EntityBase> entity);
    void registerControllableEntity(std::shared_ptr<ControllableEntity> entity);
    void registerLight(std::shared_ptr<Light> light);

    void onLogicUpdate(float deltaTime);
    void onGraphicsUpdate(DXDeviceManager *deviceManager);

    void rebuildRootEntities();
    void initLightArrayBuffer(ID3D11Device *device);

private:
    void topDownLogicUpdateRecursive(std::shared_ptr<EntityBase> entity, float deltaTime);

    void bindLightArrayBuffer(ID3D11DeviceContext *context);

    ID3D11Device *m_device;

    std::unordered_map<uint32_t, std::shared_ptr<EntityBase>> m_allEntities;
    std::unordered_map<uint32_t, std::shared_ptr<EntityBase>> m_rootEntities;
    std::unordered_map<uint32_t, std::shared_ptr<ControllableEntity>> m_controllableEntities;
    std::unordered_map<uint32_t, std::shared_ptr<Light>> m_lights;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightArrayBuffer;
};
