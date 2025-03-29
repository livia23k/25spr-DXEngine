#include "graphics/dx11/dx_swapchain.h"
#include "graphics/dx11/dx_engine.h"

DXSwapchain::DXSwapchain(DXEngine *engine, uint32_t width, uint32_t height)
    : m_engine(engine), m_device(engine->getDeviceManager()->getDevice()), m_width(width), m_height(height)
{
}

DXSwapchain::~DXSwapchain()
{
    onDestroy();
}

void DXSwapchain::onCreate(const SwapchainDesc &desc)
{
    if (!m_engine || !desc.windowHandle)
    {
        throw std::runtime_error("SwapchainDX::onCreate: Invalid engine or window handle");
    }

    DXGI_SWAP_CHAIN_DESC dxgiDesc = {};
    dxgiDesc.BufferCount = 1;
    dxgiDesc.BufferDesc.Width = (desc.width > 0) ? desc.width : 1;
    dxgiDesc.BufferDesc.Height = (desc.height > 0) ? desc.height : 1;
    dxgiDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    dxgiDesc.BufferDesc.RefreshRate.Numerator = 60;
    dxgiDesc.BufferDesc.RefreshRate.Denominator = 1;
    dxgiDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    dxgiDesc.OutputWindow = static_cast<HWND>(desc.windowHandle);
    dxgiDesc.SampleDesc.Count = 1;
    dxgiDesc.SampleDesc.Quality = 0;
    dxgiDesc.Windowed = !desc.fullscreen;
    dxgiDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    HRESULT hr = m_engine->getDeviceManager()->getFactory()->CreateSwapChain(
        m_device, &dxgiDesc, &m_swapchain);

    if (FAILED(hr))
    {
        throw std::runtime_error("SwapchainDX::onCreate: Failed to create swap chain");
    }

    onReloadBuffers(desc.width, desc.height);
}

void DXSwapchain::onDestroy()
{
    m_rtv.Reset();
    m_dsv.Reset();
    if (m_swapchain)
    {
        m_swapchain->SetFullscreenState(FALSE, nullptr);
    }
    m_swapchain.Reset();
}

void DXSwapchain::onResize(uint32_t width, uint32_t height)
{
    if (!m_swapchain)
    {
        Log(Logger::LogLevel::WARNING, "SwapchainDX::onResize: Swap chain is not created");
        return;
    }

    if (width == 0 || height == 0)
    {
        Log(Logger::LogLevel::WARNING, "SwapchainDX::onResize: Invalid width or height");
        return;
    }

    m_rtv.Reset();
    m_dsv.Reset();

    HRESULT hr = m_swapchain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr))
    {
        throw std::runtime_error(std::format("SwapchainDX::onResize: Failed to resize buffers, HRESULT = {:#X}", hr));
    }

    onReloadBuffers(width, height);
}

void DXSwapchain::onSetFullscreen(bool fullscreen, uint32_t width, uint32_t height)
{
    if (!m_swapchain)
    {
        Log(Logger::LogLevel::WARNING, "SwapchainDX::onSetFullscreen: Swap chain is not created");
        return;
    }

    onResize(width, height);

    HRESULT hr = m_swapchain->SetFullscreenState(fullscreen, nullptr);
    if (FAILED(hr))
    {
        throw std::runtime_error(std::format("SwapchainDX::onSetFullscreen: Failed to set fullscreen state, HRESULT = {:#X}", hr));
    }
}

bool DXSwapchain::onPresent(bool vsync)
{
    if (!m_swapchain)
    {
        Log(Logger::LogLevel::WARNING, "SwapchainDX::onPresent: Swap chain is not created");
        return false;
    }

    HRESULT hr = m_swapchain->Present(vsync ? 1 : 0, 0);
    if (FAILED(hr))
    {
        throw std::runtime_error(std::format("SwapchainDX::onPresent: Failed to present swap chain, HRESULT = {:#X}", hr));
    }

    return true;
}

void DXSwapchain::onReloadBuffers(uint32_t width, uint32_t height)
{
    if (!m_swapchain || !m_device)
    {
        Log(Logger::LogLevel::WARNING, "SwapchainDX::onReloadBuffers: Swap chain or device is not created");
        return;
    }

    // Get back buffer
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
    {
        throw std::runtime_error(std::format("SwapchainDX::onReloadBuffers: Failed to get back buffer, HRESULT = {:#X}", hr));
    }

    // (Re-)Create render target view
    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_rtv);
    if (FAILED(hr))
    {
        throw std::runtime_error(std::format("SwapchainDX::onReloadBuffers: Failed to create render target view, HRESULT = {:#X}", hr));
    }

    // (Re-)Create depth buffer
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer;
    hr = m_device->CreateTexture2D(&depthDesc, nullptr, &depthBuffer);
    if (FAILED(hr))
    {
        throw std::runtime_error(std::format("SwapchainDX::onReloadBuffers: Failed to create depth buffer, HRESULT = {:#X}", hr));
    }

    // (Re-)Create depth stencil view
    hr = m_device->CreateDepthStencilView(depthBuffer.Get(), nullptr, &m_dsv);
    if (FAILED(hr))
    {
        throw std::runtime_error(std::format("SwapchainDX::onReloadBuffers: Failed to create depth stencil view, HRESULT = {:#X}", hr));
    }
}
