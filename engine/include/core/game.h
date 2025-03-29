#pragma once

#include "utils/forward.h"
#include "entity/camera_base.h"
#include "entity/camera_fp.h"
#include "entity/camera_tp.h"
#include "core/input_manager.h"
#include "core/window_win.h"
#include "graphics/dx11/dx_engine.h"
#include "resources/game_resource_mgr.h"

class Game
{
public:
    Game(uint32_t width, uint32_t height, const std::string &title);
    virtual ~Game() = default;

    void setControlledEntity(std::shared_ptr<ControllableEntity> entity);

    virtual void onCreate() = 0;
    void run();

protected:
    void onDestroy();

    virtual void onGraphicsUpdate(float deltaTime);
    virtual void onLogicUpdate(float deltaTime);
    virtual void onInputUpdate(float deltaTime);

    void onWindowZoom(float delta);
    void switchCamera(bool is_first_person_camera);

    bool m_isRunning;
    bool m_isPaused;

    std::shared_ptr<ControllableEntity> m_controlledEntity;

    bool m_isFPCameraActive;
    std::shared_ptr<CameraBase> m_activeCamera;
    std::shared_ptr<FirstPersonCamera> m_firstPersonCamera;
    std::shared_ptr<ThirdPersonCamera> m_thirdPersonCamera;

    std::unique_ptr<InputManager> m_input;
    std::unique_ptr<WindowWin> m_window;
    std::shared_ptr<DXEngine> m_graphicsEngine;
    std::shared_ptr<GameResourceManager> m_gameResourceManager;
};
