#include "graphics/dx11/dx_device_mgr.h"

DXDeviceManager::DXDeviceManager()
{
}

DXDeviceManager::~DXDeviceManager()
{
    destroyDevice();
}

void DXDeviceManager::createDevice()
{
    // Create d3d device
    HRESULT hr = D3D11CreateDevice(
        nullptr,                                                      // video dxgi_adapter
        D3D_DRIVER_TYPE_HARDWARE,                                     // driver
        nullptr,                                                      // software rasterizer
        D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT, // device flag
        nullptr, 0,                                                   // feature level: DX11 by default
        D3D11_SDK_VERSION,                                            // sdk version
        &m_d3dDevice,                                                 // device
        nullptr,                                                      // succeeded feature level
        &m_d3dDeviceContext                                           // device context
    );
    if (FAILED(hr))
    {
        throw std::runtime_error("DXDeviceManager::createDevice: Failed to create d3d device");
    }

    // Create dxgi device
    hr = m_d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void **>(m_dxgiDevice.GetAddressOf()));
    if (FAILED(hr))
    {
        throw std::runtime_error("DXDeviceManager::createDevice: Failed to create dxgi device");
    }

    // Create dxgi adapter
    hr = m_dxgiDevice->GetAdapter(m_dxgiAdapter.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("DXDeviceManager::createDevice: Failed to create dxgi adapter");
    }

    // Create dxgi factory
    hr = m_dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void **>(m_dxgiFactory.GetAddressOf()));
    if (FAILED(hr))
    {
        throw std::runtime_error("DXDeviceManager::createDevice: Failed to create dxgi factory");
    }

    // Create rasterizer state
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.FrontCounterClockwise = false; // left-hand, cw TODO: check;
    rasterDesc.DepthClipEnable = true;
    hr = m_d3dDevice->CreateRasterizerState(&rasterDesc, m_rasterStateCullBack.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("DXDeviceManager::createDevice: Failed to create cull back rasterizer state!");
    }
    rasterDesc.CullMode = D3D11_CULL_FRONT;
    hr = m_d3dDevice->CreateRasterizerState(&rasterDesc, m_rasterStateCullFront.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("DXDeviceManager::createDevice: Failed to create cull front rasterizer state!");
    }
}

void DXDeviceManager::destroyDevice()
{
    m_d3dDeviceContext.Reset();
    m_d3dDevice.Reset();
    m_dxgiDevice.Reset();
    m_dxgiAdapter.Reset();
    m_dxgiFactory.Reset();
    m_rasterStateCullBack.Reset();
    m_rasterStateCullFront.Reset();
}

Microsoft::WRL::ComPtr<ID3D11Buffer> DXDeviceManager::createVertexBuffer(const void *data, uint32_t size)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = size;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    HRESULT hr = m_d3dDevice->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create vertex buffer.");
    }

    return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> DXDeviceManager::createIndexBuffer(const void *data, uint32_t size)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = size;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = data;

    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    HRESULT hr = m_d3dDevice->CreateBuffer(&bufferDesc, &initData, &indexBuffer);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create index buffer.");
    }

    return indexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> DXDeviceManager::createConstantBuffer(uint32_t size)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = size;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
    HRESULT hr = m_d3dDevice->CreateBuffer(&bufferDesc, nullptr, &constantBuffer);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create constant buffer.");
    }

    return constantBuffer;
}

void DXDeviceManager::setRasterStateCullBack()
{
    m_d3dDeviceContext->RSSetState(m_rasterStateCullBack.Get());
}

void DXDeviceManager::setRasterStateCullFront()
{
    m_d3dDeviceContext->RSSetState(m_rasterStateCullFront.Get());
}
