#pragma once

#include "utils/forward.h"
#include <vector>
#include <string>

class Mesh
{
public:
    Mesh(ID3D11Device *device, const std::string &filepath, const std::string &name);
    Mesh(ID3D11Device *device, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const std::string &name);
    ~Mesh();

    const UINT getIndicesCount() const;

    void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology);

    void loadFromOBJ(const std::string &filepath);
    void bind(ID3D11DeviceContext *deviceContext) const;

private:
    void initBuffers(ID3D11Device *device, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

    std::string m_name;

    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};
