#pragma once
#include "utils/forward.h"

struct Vertex {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 n;
    DirectX::XMFLOAT2 uv;

    Vertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 normal, DirectX::XMFLOAT2 texcoord)
        : pos(p), n(normal), uv(texcoord) {}

    static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
    static const uint32_t numElements = ARRAYSIZE(inputLayout);
};
