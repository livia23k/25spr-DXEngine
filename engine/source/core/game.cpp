#include "core/game.h"
#include "resources/vertex.h"
#include "resources/mesh.h"
#include "resources/material.h"
#include "resources/shader.h"
#include "resources/render_component.h"
#include "graphics/dx11/dx_device_mgr.h"

#include "entity/entity_controllable.h"

Game::Game(uint32_t width, uint32_t height, const std::string &title)
    : m_isRunning(true), m_isPaused(false)
{
    m_window = std::make_unique<WindowWin>(width, height, title);
    m_input = std::make_unique<InputManager>();
    m_graphicsEngine = std::make_unique<DXEngine>(m_window.get());
    m_graphicsEngine->onCreate();
    m_gameResourceManager = std::make_shared<GameResourceManager>(m_graphicsEngine->getDeviceManager()->getDevice());
    m_window->setDestroyCallback([this]()
                                 { onDestroy(); });
    m_window->setZoomCallback([this](float delta)
                              { onWindowZoom(delta); });
    m_graphicsEngine->setGameResourceManager(m_gameResourceManager);
}

void Game::run()
{
    m_window->onCreate();

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (m_isRunning)
    {
        m_window->onProcessEvents();

        if (m_isPaused)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        onInputUpdate(deltaTime);
        onLogicUpdate(deltaTime);
        onGraphicsUpdate(deltaTime);
    }

    m_window->onDestroy();
}

void Game::onCreate()
{
    throw std::runtime_error("Game::onCreate: not overridden");
}

void Game::onDestroy()
{
    m_isRunning = false;
    m_graphicsEngine->onDestroy();
}

void Game::onInputUpdate(float deltaTime)
{
    m_input->updateStates();

    if (m_input->isKeyDown(KeyCode::Escape))
    {
        m_isRunning = false;
    }

    // camera
    if (m_input->isKeyDown(KeyCode::F1))
    {
        switchCamera(true);
    }
    else if (m_input->isKeyDown(KeyCode::F2))
    {
        switchCamera(false);
    }

    // view control by dragging
    if (m_input->isMouseHeld(KeyCode::MouseLeft))
    {
        m_activeCamera->onMouseDrag(m_input->getMouseDeltaX(), m_input->getMouseDeltaY());
    }

    // move control
    if (m_input->isKeyHeld(KeyCode::W))
    {
        m_controlledEntity->move(deltaTime, ControllableEntity::MovementDirection::Forward);
    }
    else if (m_input->isKeyHeld(KeyCode::S))
    {
        m_controlledEntity->move(deltaTime, ControllableEntity::MovementDirection::Backward);
    }
    if (m_input->isKeyHeld(KeyCode::A))
    {
        m_controlledEntity->move(deltaTime, ControllableEntity::MovementDirection::Left);
    }
    else if (m_input->isKeyHeld(KeyCode::D))
    {
        m_controlledEntity->move(deltaTime, ControllableEntity::MovementDirection::Right);
    }
    if (m_input->isKeyHeld(KeyCode::Q))
    {
        m_controlledEntity->move(deltaTime, ControllableEntity::MovementDirection::Up);
    }
    else if (m_input->isKeyHeld(KeyCode::E))
    {
        m_controlledEntity->move(deltaTime, ControllableEntity::MovementDirection::Down);
    }

    // rotate control
    if (m_input->isKeyHeld(KeyCode::Left))
    {
        m_controlledEntity->rotate(deltaTime, ControllableEntity::RotationDirection::YawLeft);
    }
    else if (m_input->isKeyHeld(KeyCode::Right))
    {
        m_controlledEntity->rotate(deltaTime, ControllableEntity::RotationDirection::YawRight);
    }
    if (m_input->isKeyHeld(KeyCode::Up))
    {
        m_controlledEntity->rotate(deltaTime, ControllableEntity::RotationDirection::PitchUp);
    }
    else if (m_input->isKeyHeld(KeyCode::Down))
    {
        m_controlledEntity->rotate(deltaTime, ControllableEntity::RotationDirection::PitchDown);
    }
}

void Game::onLogicUpdate(float deltaTime)
{
    m_gameResourceManager->onLogicUpdate(deltaTime);
    m_activeCamera->onLogicUpdate(deltaTime);
}

void Game::onGraphicsUpdate(float deltaTime)
{
    m_graphicsEngine->beginRender();
    m_activeCamera->onGraphicsUpdate(m_graphicsEngine->getDeviceManager()->getDeviceContext());
    m_gameResourceManager->onGraphicsUpdate(m_graphicsEngine->getDeviceManager());
    m_graphicsEngine->endRender();
}

void Game::onWindowZoom(float delta)
{
    m_activeCamera->onZoom(delta);
}

void Game::switchCamera(bool is_first_person_camera)
{
    m_isFPCameraActive = is_first_person_camera;
    m_activeCamera = is_first_person_camera ? std::static_pointer_cast<CameraBase>(m_firstPersonCamera) : std::static_pointer_cast<CameraBase>(m_thirdPersonCamera);

    if (is_first_person_camera)
    {
        Logger::LogInfo("Game::switchCamera: switch to first person camera");
    }
    else
    {
        Logger::LogInfo("Game::switchCamera: switch to third person camera");
    }
}
