#pragma once

#include "utils/forward.h"
#include "graphics/dx11/dx_device_mgr.h"
#include "graphics/dx11/dx_swapchain.h"
#include "core/window_win.h"

class DXEngine
{
public:
    DXEngine(WindowWin *window);
    virtual ~DXEngine();

    void onCreate();
    void onDestroy();
    void onResize(uint32_t width, uint32_t height);

    void beginRender();
    void endRender();

    void setGameResourceManager(std::shared_ptr<GameResourceManager> game_resource_manager) { m_gameResourceManager = game_resource_manager; }

    DXDeviceManager *getDeviceManager() const { return m_deviceManager.get(); }
    DXSwapchain *getSwapchain() const { return m_swapchain.get(); }

private:
    std::unique_ptr<DXDeviceManager> m_deviceManager;
    std::unique_ptr<DXSwapchain> m_swapchain;
    std::shared_ptr<GameResourceManager> m_gameResourceManager;
    WindowWin *m_window;
};