#pragma once

#include "utils/forward.h"
#include "resources/buffer_type.h"

class Light
{
public:
    enum class Type
    {
        Point = 0
    };

    Light(Type type, const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &color, float intensity);

    uint32_t getId() const;
    Type getType() const;
    DirectX::XMFLOAT3 getPosition() const;
    DirectX::XMFLOAT3 getColor() const;
    float getIntensity() const;
    LightBuffer getLightBuffer() const;

    void setPosition(const DirectX::XMFLOAT3 &position);
    void setColor(const DirectX::XMFLOAT3 &color);
    void setIntensity(float intensity);

    // void initLightBuffer(ID3D11Device *device);
    // void bindLightBuffer(ID3D11DeviceContext *context);

private:
    static std::atomic<uint32_t> s_nextId;

    uint32_t m_id;
    Type m_type;
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_color;
    float m_intensity;

    // Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightBuffer;
};