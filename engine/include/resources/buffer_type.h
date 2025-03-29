#pragma once

#include "utils/forward.h"

struct CameraBuffer
{
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
};

struct ModelBuffer
{
    DirectX::XMMATRIX world;
};

struct alignas(16) MaterialBuffer
{
    DirectX::XMFLOAT4 albedo;
    int useTexture;
    int padding[3];
};
static_assert(sizeof(MaterialBuffer) == 32, "MaterialBuffer size mismatch!");

struct alignas(16) LightBuffer
{
    DirectX::XMFLOAT4 position; // .xyz = position, .w = type
    DirectX::XMFLOAT4 color;    // .xyz = color, .w = intensity
};
static_assert(sizeof(LightBuffer) == 32, "LightBuffer size mismatch!");

constexpr int MAX_LIGHTS = 1;
struct alignas(16) LightArrayBuffer
{
    LightBuffer lights[MAX_LIGHTS];
    int numLights;
    int padding[3];
};
static_assert(sizeof(LightArrayBuffer) == 48, "LightArrayBuffer size mismatch!");
