#include "resources/material.h"

LambertianMaterial::LambertianMaterial(ID3D11Device *device, std::shared_ptr<Shader> shader, const DirectX::XMFLOAT4 &albedo)
    : m_shader(shader), m_albedoTexture(nullptr)
{
    MaterialBuffer materialData;
    materialData.albedo = albedo;
    materialData.useTexture = 0;
    materialData.padding[0] = 0;
    materialData.padding[1] = 0;
    materialData.padding[2] = 0;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(MaterialBuffer);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &materialData;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &m_constantBuffer);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create constant buffer for Lambertian material.");
    }
}

LambertianMaterial::LambertianMaterial(ID3D11Device *device, std::shared_ptr<Shader> shader, const std::string &albedoPath)
    : m_shader(shader)
{
    m_albedoTexture = std::make_shared<ImageTexture>(device, albedoPath);

    MaterialBuffer materialData;
    materialData.albedo = DirectX::XMFLOAT4(1.f, 0.0f, 1.0f, 1.0f);
    materialData.useTexture = 1;
    materialData.padding[0] = 0;
    materialData.padding[1] = 0;
    materialData.padding[2] = 0;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = sizeof(MaterialBuffer);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &materialData;

    HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &m_constantBuffer);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create constant buffer for Lambertian material.");
    }
}

void LambertianMaterial::bind(ID3D11DeviceContext *deviceContext) const
{
    if (m_shader)
    {
        m_shader->bind(deviceContext);
    }

    // bind constant buffer to slot 2 (matches register(b2))
    deviceContext->PSSetConstantBuffers(2, 1, m_constantBuffer.GetAddressOf());

    // bind albedo texture to slot 2 if available (matches register(t2))
    if (m_albedoTexture)
    {
        m_albedoTexture->bind(deviceContext, 2);
    }
}

void LambertianMaterial::setProperty(const std::string &name, const void *data, size_t size)
{
    if (name == "albedo" && size == sizeof(DirectX::XMFLOAT4))
    {
        Logger::Log(Logger::LogLevel::WARNING, "LambertianMaterial:setProperty: Cannot modify albedo of an immutable material");
    }
    else
    {
        Logger::Log(Logger::LogLevel::WARNING, "LambertianMaterial:setProperty: Invalid property name: {}", name);
    }
}
