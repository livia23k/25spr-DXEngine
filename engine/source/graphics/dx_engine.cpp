#include "graphics/dx11/dx_engine.h"
#include "graphics/dx11/dx_device_mgr.h"
#include "graphics/dx11/dx_swapchain.h"
#include "resources/game_resource_mgr.h"

DXEngine::DXEngine(WindowWin *window)
    : m_window(window)
{
    if (m_window)
    {
        m_window->setResizeCallback([this](uint32_t width, uint32_t height)
                                    { this->onResize(width, height); });
    }
}

DXEngine::~DXEngine()
{
    onDestroy();
}

void DXEngine::onCreate()
{
    m_deviceManager = std::make_unique<DXDeviceManager>();
    m_deviceManager->createDevice();

    SwapchainDesc desc = {};
    desc.windowHandle = m_window->getNativeHandle();
    desc.width = m_window->getWidth();
    desc.height = m_window->getHeight();
    desc.fullscreen = false;

    m_swapchain = std::make_unique<DXSwapchain>(this, m_window->getWidth(), m_window->getHeight());
    m_swapchain->onCreate(desc);
}

void DXEngine::onDestroy()
{
    if (m_swapchain)
    {
        m_swapchain->onDestroy();
        m_swapchain.reset();
    }

    if (m_deviceManager)
    {
        m_deviceManager->destroyDevice();
        m_deviceManager.reset();
    }
}

void DXEngine::onResize(uint32_t width, uint32_t height)
{
    if (!m_swapchain)
    {
        Log(Logger::LogLevel::WARNING, "DXEngine::onResize: Swap chain is not created");
        return;
    }

    m_swapchain->onResize(width, height);
}

void DXEngine::beginRender()
{
    if (!m_swapchain)
    {
        Log(Logger::LogLevel::WARNING, "DXEngine::beginRender: Swap chain is not created");
        return;
    }

    if (!m_deviceManager)
    {
        Log(Logger::LogLevel::WARNING, "DXEngine::beginRender: Device manager is not created");
        return;
    }

    auto deviceContext = m_deviceManager->getDeviceContext();
    auto rtv = m_swapchain->getRenderTargetView();
    auto dsv = m_swapchain->getDepthStencilView();

    // RTV & DSV (clear and set)
    const float clearColor[4] = {0.8f, 0.8f, 0.8f, 1.0f}; // grey
    deviceContext->ClearRenderTargetView(rtv, clearColor);
    deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    deviceContext->OMSetRenderTargets(1, &rtv, dsv);
    deviceContext->OMSetDepthStencilState(nullptr, 0);

    // Viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_swapchain->getWidth());
    viewport.Height = static_cast<float>(m_swapchain->getHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    deviceContext->RSSetViewports(1, &viewport);
}

void DXEngine::endRender()
{
    if (!m_swapchain)
    {
        Log(Logger::LogLevel::WARNING, "DXEngine::onRender: Swap chain is not created");
        return;
    }

    // Submit
    m_swapchain->onPresent(true);
}
