#include "entity/light.h"

std::atomic<uint32_t> Light::s_nextId = 0;

Light::Light(Type type, const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &color, float intensity)
    : m_id(s_nextId.fetch_add(1)),
      m_type(type),
      m_position(position),
      m_color(color),
      m_intensity(intensity)
{
}

uint32_t Light::getId() const { return m_id; }
DirectX::XMFLOAT3 Light::getPosition() const { return m_position; }
DirectX::XMFLOAT3 Light::getColor() const { return m_color; }
float Light::getIntensity() const { return m_intensity; }

LightBuffer Light::getLightBuffer() const
{
    LightBuffer lightBuffer = {};
    lightBuffer.position = {m_position.x, m_position.y, m_position.z, (float)m_type};
    lightBuffer.color = {m_color.x, m_color.y, m_color.z, m_intensity};
    return lightBuffer;
}

void Light::setPosition(const DirectX::XMFLOAT3 &position) { m_position = position; }
void Light::setColor(const DirectX::XMFLOAT3 &color) { m_color = color; }
void Light::setIntensity(float intensity) { m_intensity = intensity; }

// void Light::initLightBuffer(ID3D11Device *device) {
//     if (!device) {
//         throw std::runtime_error("Light::initLightBuffer: device is nullptr");
//     }

//     D3D11_BUFFER_DESC bufferDesc;
//     bufferDesc.ByteWidth = sizeof(LightBuffer);
//     bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//     bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//     bufferDesc.CPUAccessFlags = 0;
//     bufferDesc.MiscFlags = 0;
//     bufferDesc.StructureByteStride = 0;

//     LightBuffer lightBuffer = getLightBuffer();

//     D3D11_SUBRESOURCE_DATA initData;
//     initData.pSysMem = &lightBuffer;
//     initData.SysMemPitch = 0;
//     initData.SysMemSlicePitch = 0;

//     HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &m_lightBuffer);
//     if (FAILED(hr)) {
//         throw std::runtime_error("Light::initLightBuffer: failed to create light buffer");
//     }
// }

// void Light::bindLightBuffer(ID3D11DeviceContext *context) {
//     if (!context) {
//         throw std::runtime_error("Light::bindLightBuffer: context is nullptr");
//     }

//     // LightBuffer lightBuffer = getLightBuffer();
//     // context->UpdateSubresource(m_lightBuffer.Get(), 0, nullptr, &lightBuffer, 0, 0);
//     context->PSSetConstantBuffers(3, 1, m_lightBuffer.GetAddressOf());
// }
