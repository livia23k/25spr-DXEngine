#pragma once

#include "utils/forward.h"
#include <d3d11.h>
#include <wrl/client.h>

struct SwapchainDesc
{
    void *windowHandle = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    bool fullscreen = false;
};

class DXSwapchain
{
public:
    DXSwapchain(DXEngine *engine, uint32_t width, uint32_t height);
    ~DXSwapchain();

    void onCreate(const SwapchainDesc &desc);
    void onDestroy();
    void onResize(uint32_t width, uint32_t height);
    void onSetFullscreen(bool fullscreen, uint32_t width, uint32_t height);
    bool onPresent(bool vsync);
    void onReloadBuffers(uint32_t width, uint32_t height);

    ID3D11RenderTargetView *getRenderTargetView() const { return m_rtv.Get(); }
    ID3D11DepthStencilView *getDepthStencilView() const { return m_dsv.Get(); }

    ID3D11RenderTargetView *const *getRendgerTargetViewAddress() const { return m_rtv.GetAddressOf(); }
    ID3D11DepthStencilView *const *getDepthStencilViewAddress() const { return m_dsv.GetAddressOf(); }

    uint32_t getWidth() const { return m_width; }
    uint32_t getHeight() const { return m_height; }

private:
    DXEngine *m_engine;
    ID3D11Device *m_device;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapchain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv; // render target view
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv; // depth stencil view

    uint32_t m_width;
    uint32_t m_height;
    bool m_isFullscreen;
};