#include "resources/texture.h"
#include "external/DirectXTex/DirectXTex.h"

ConstantTexture::ConstantTexture(ID3D11Device *device, const DirectX::XMFLOAT4 &color)
{
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = 1;
    desc.Height = 1;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    UINT32 colorData =
        (static_cast<UINT32>(color.x * 255) << 0) | // TODO: check
        (static_cast<UINT32>(color.y * 255) << 8) |
        (static_cast<UINT32>(color.z * 255) << 16) |
        (static_cast<UINT32>(color.w * 255) << 24);

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &colorData;
    initData.SysMemPitch = sizeof(UINT32);

    HRESULT hr = device->CreateTexture2D(&desc, &initData, m_texture.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create constant texture.");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_srv.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create shader resource view for constant texture.");
    }
}

void ConstantTexture::bind(ID3D11DeviceContext *deviceContext, UINT slot) const
{
    deviceContext->PSSetShaderResources(slot, 1, m_srv.GetAddressOf());
}

ImageTexture::ImageTexture(ID3D11Device *device, const std::string &filePath)
{
    loadFromFile(device, filePath);
}

void ImageTexture::bind(ID3D11DeviceContext *deviceContext, UINT slot) const
{
    deviceContext->PSSetShaderResources(slot, 1, m_srv.GetAddressOf());
}

void ImageTexture::loadFromFile(ID3D11Device *device, const std::string &filePath)
{
    DirectX::ScratchImage scratchImage;
    HRESULT hr = DirectX::LoadFromWICFile(
        std::wstring(filePath.begin(), filePath.end()).c_str(),
        DirectX::WIC_FLAGS_NONE,
        nullptr,
        scratchImage);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to load texture from file: " + filePath);
    }

    Microsoft::WRL::ComPtr<ID3D11Resource> textureResource;
    hr = DirectX::CreateTexture(
        device,
        scratchImage.GetImages(),
        scratchImage.GetImageCount(),
        scratchImage.GetMetadata(),
        textureResource.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create texture from image data.");
    }
    hr = textureResource.As(&m_texture);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to convert ID3D11Resource to ID3D11Texture2D.");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = scratchImage.GetMetadata().format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    hr = device->CreateShaderResourceView(
        m_texture.Get(),
        &srvDesc,
        &m_srv);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create shader resource view.");
    }
}
