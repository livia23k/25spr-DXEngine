#include "game_3dbasic.h"
#include "resources/vertex.h"
#include "celestial_body.h"
#include "spaceship.h"

Game3DBasic::Game3DBasic(uint32_t width, uint32_t height, const std::string &title)
    : Game(width, height, title) {}

void Game3DBasic::onCreate()
{
    auto device = m_graphicsEngine->getDeviceManager()->getDevice();

    // test entity
    // std::vector<Vertex> vertices = {
    //     { { -1.f,  -1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.f, 1.f} },  // left bottom
    //     { { 0.f, 1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.0f} },   // top
    //     { { 1.f, -1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.f, 1.f} }  // right bottom
    // };
    // std::vector<uint32_t> indices = {0, 1, 2};
    // auto meshTriangle = std::make_shared<Mesh>(device, vertices, indices, "lambertian_red"); // mesh: triangle

    // init material

    // (shader)
    auto shaderBase = std::make_shared<Shader>(device, L"engine/assets/shader/vs_base.hlsl", L"engine/assets/shader/ps_base.hlsl", Vertex::inputLayout, Vertex::numElements);
    auto shaderSkybox = std::make_shared<Shader>(device, L"engine/assets/shader/vs_skybox.hlsl", L"engine/assets/shader/ps_skybox.hlsl", Vertex::inputLayout, Vertex::numElements);

    // (material)
    std::shared_ptr<MaterialBase> matLambertianGrey = std::make_shared<LambertianMaterial>(device, shaderBase, DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
    std::shared_ptr<MaterialBase> matLambertianChecker1 = std::make_shared<LambertianMaterial>(device, shaderBase, "engine/assets/texture/checker.png");
    std::shared_ptr<MaterialBase> matLambertianChecker2 = std::make_shared<LambertianMaterial>(device, shaderBase, "engine/assets/texture/checker-map_tho.png");
    std::shared_ptr<MaterialBase> matLambertianSun = std::make_shared<LambertianMaterial>(device, shaderBase, "game/celestial_rover/assets/texture/sun.png");
    std::shared_ptr<MaterialBase> matLambertianEarth = std::make_shared<LambertianMaterial>(device, shaderBase, "game/celestial_rover/assets/texture/earth.png");
    std::shared_ptr<MaterialBase> matLambertianMoon = std::make_shared<LambertianMaterial>(device, shaderBase, "game/celestial_rover/assets/texture/moon.png");
    std::shared_ptr<MaterialBase> matSkybox = std::make_shared<LambertianMaterial>(device, shaderSkybox, "game/celestial_rover/assets/texture/galaxy.png");

    // init mesh

    auto meshSpaceShip = std::make_shared<Mesh>(device, "game/celestial_rover/assets/mesh/spaceship.obj", "spaceship");
    auto meshSphere = std::make_shared<Mesh>(device, "game/celestial_rover/assets/mesh/sphere.obj", "sphere");
    auto meshCube = std::make_shared<Mesh>(device, "game/celestial_rover/assets/mesh/cube.obj", "cube");

    // init render component

    auto compSun = std::make_shared<RenderComponent>(meshSphere, matLambertianSun);
    auto compEarth = std::make_shared<RenderComponent>(meshSphere, matLambertianEarth);
    auto compMoon = std::make_shared<RenderComponent>(meshSphere, matLambertianMoon);
    auto compSpaceShip = std::make_shared<RenderComponent>(meshSpaceShip, matLambertianGrey);
    auto compSkybox = std::make_shared<RenderComponent>(meshCube, matSkybox);

    // init entities

    // (orbit)
    auto orbitEarthSun = std::make_shared<Orbit>(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 400.f, 0.5f, 0.07f);
    auto orbitMoonEarth = std::make_shared<Orbit>(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 20.0f, 0.5f, 0.2f);
    auto orbitShipEarth = std::make_shared<Orbit>(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 10.0f, 0.2f, 1.f);

    // (sun)
    auto entitySun = std::make_shared<CelestialBody>(device);
    entitySun->addRenderComponent(compSun);
    entitySun->setLocalPosition(DirectX::XMFLOAT3(500.0f, 500.0f, 500.0f));
    entitySun->setLocalScale(DirectX::XMFLOAT3(50.f, 50.f, 50.f));
    entitySun->setSelfRotationSpeed(0.04f);

    // (earth)
    auto entityEarth = std::make_shared<CelestialBody>(device);
    entityEarth->addRenderComponent(compEarth);
    entityEarth->setLocalScale(DirectX::XMFLOAT3(5.f, 5.f, 5.f));
    entityEarth->setOrbit(orbitEarthSun);
    entityEarth->setPrimaryBody(entitySun);
    entityEarth->setSelfRotationSpeed(1.f);

    // (moon)
    auto entityMoon = std::make_shared<CelestialBody>(device);
    entityMoon->addRenderComponent(compMoon);
    entityMoon->setLocalScale(DirectX::XMFLOAT3(3.f, 3.f, 3.f));
    entityMoon->setOrbit(orbitMoonEarth);
    entityMoon->setPrimaryBody(entityEarth);
    entityMoon->setSelfRotationSpeed(0.037f);

    // (spaceship)
    auto shipEarthOrbit = std::make_shared<Spaceship>(device);
    shipEarthOrbit->addRenderComponent(compSpaceShip);
    shipEarthOrbit->setLocalScale(DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f));
    shipEarthOrbit->setMoveSpeed(50.f);
    shipEarthOrbit->setTransferSpeed(0.5f);
    shipEarthOrbit->setState(Spaceship::State::Orbiting, entityEarth, orbitShipEarth);

    // (skybox)
    compSkybox->setIsCullFront(false);
    auto entitySkybox = std::make_shared<EntityBase>(device);
    entitySkybox->addRenderComponent(compSkybox);
    entitySkybox->setLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
    entitySkybox->setLocalScale(DirectX::XMFLOAT3(5000.0f, 5000.0f, 5000.0f));

    // init light

    auto light = std::make_shared<Light>(Light::Type::Point, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.0f);
   
    // register entities

    m_gameResourceManager->registerStaticEntity(entitySun);
    m_gameResourceManager->registerStaticEntity(entityEarth);
    m_gameResourceManager->registerStaticEntity(entityMoon);
    m_gameResourceManager->registerControllableEntity(shipEarthOrbit);
    m_gameResourceManager->registerStaticEntity(entitySkybox);
    m_gameResourceManager->registerLight(light);

    m_controlledEntity = m_gameResourceManager->getControllableEntity(shipEarthOrbit->getId());
    if (m_controlledEntity == nullptr) { throw std::runtime_error("Game3DBasic::onCreate: m_controlledEntity is nullptr"); }

    // init Camera

    m_firstPersonCamera = std::make_shared<FirstPersonCamera>(device, m_controlledEntity.get());
    m_firstPersonCamera->setPositionOffset(DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f));
    m_thirdPersonCamera = std::make_shared<ThirdPersonCamera>(device, m_controlledEntity.get());
    m_activeCamera = m_firstPersonCamera;
    m_isFPCameraActive = true;

    // other preparations

    m_gameResourceManager->rebuildRootEntities();
    m_gameResourceManager->initLightArrayBuffer(device);

}

void Game3DBasic::onLogicUpdate(float deltaTime)
{
    Game::onLogicUpdate(deltaTime);

    // update light position to the sun
    auto entitySun = std::dynamic_pointer_cast<CelestialBody>(m_gameResourceManager->getEntity(0));
    if (!entitySun)
    {
        Logger::LogError("Game3DBasic::onLogicUpdate: entitySun is nullptr");
        return;
    }
    auto light = m_gameResourceManager->getLight(0);
    if (light)
    {
        light->setPosition(entitySun->getWorldPosition());
    }
}

void Game3DBasic::onInputUpdate(float deltaTime)
{
    Game::onInputUpdate(deltaTime);

    std::shared_ptr<Spaceship> spaceship = std::dynamic_pointer_cast<Spaceship>(m_gameResourceManager->getControllableEntity(3));
    if (!spaceship)
    {
        Logger::LogError("Game3DBasic::onInputUpdate: spaceship is nullptr");
        return;
    }

    auto entityEarth = std::dynamic_pointer_cast<CelestialBody>(m_gameResourceManager->getEntity(1));
    auto entityMoon = std::dynamic_pointer_cast<CelestialBody>(m_gameResourceManager->getEntity(2));
    if (!entityEarth || !entityMoon)
    {
        Logger::LogError("Game3DBasic::onInputUpdate: entityEarth or entityMoon is nullptr");
        return;
    }

    // update spaceship state
    if (m_input->isKeyDown(KeyCode::Num0))
    {
        spaceship->setState(Spaceship::State::Free);
    }
    else if (m_input->isKeyDown(KeyCode::Num1))
    {
        Logger::LogInfo("Landing on Earth");
        spaceship->setState(Spaceship::State::Landing, entityEarth);
    }
    else if (m_input->isKeyDown(KeyCode::Num2))
    {
        Logger::LogInfo("Landing on Moon");
        spaceship->setState(Spaceship::State::Landing, entityMoon);
    }
    else if (m_input->isKeyDown(KeyCode::Num3))
    {
        Logger::LogInfo("Orbiting Earth");
        auto orbitEarth = std::make_shared<Orbit>(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 10.0f, 0.2f, 1.f);
        spaceship->setState(Spaceship::State::Orbiting, entityEarth, orbitEarth);
    }
    else if (m_input->isKeyDown(KeyCode::Num4))
    {
        Logger::LogInfo("Orbiting Moon");
        auto orbitMoon = std::make_shared<Orbit>(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 6.0f, 0.5f, 1.2f);
        spaceship->setState(Spaceship::State::Orbiting, entityMoon, orbitMoon);
    }
}
