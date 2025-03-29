#pragma once

#include "utils/forward.h"
#include <d3d11.h>
#include <wrl/client.h>

class DXDeviceManager
{
public:
    DXDeviceManager();
    ~DXDeviceManager();

    void createDevice();
    void destroyDevice();

    ID3D11Device *getDevice() const { return m_d3dDevice.Get(); }
    ID3D11DeviceContext *getDeviceContext() const { return m_d3dDeviceContext.Get(); }
    IDXGIAdapter *getAdapter() const { return m_dxgiAdapter.Get(); }
    IDXGIFactory *getFactory() const { return m_dxgiFactory.Get(); }

    Microsoft::WRL::ComPtr<ID3D11Buffer> createVertexBuffer(const void *data, uint32_t size);
    Microsoft::WRL::ComPtr<ID3D11Buffer> createIndexBuffer(const void *data, uint32_t size);
    Microsoft::WRL::ComPtr<ID3D11Buffer> createConstantBuffer(uint32_t size);

    void setRasterStateCullBack();
    void setRasterStateCullFront();

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dDeviceContext;
    Microsoft::WRL::ComPtr<IDXGIDevice> m_dxgiDevice;
    Microsoft::WRL::ComPtr<IDXGIAdapter> m_dxgiAdapter;
    Microsoft::WRL::ComPtr<IDXGIFactory> m_dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterStateCullBack;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterStateCullFront;
};
