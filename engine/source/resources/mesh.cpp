#include "resources/vertex.h"
#include "resources/mesh.h"
#include <fstream>
#include <sstream>

Mesh::Mesh(ID3D11Device *device, const std::string &filepath, const std::string &name)
    : m_name(name), m_primitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
    loadFromOBJ(filepath);
    initBuffers(device, m_vertices, m_indices);
}

Mesh::Mesh(ID3D11Device *device, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const std::string &name)
    : m_name(name), m_vertices(vertices), m_indices(indices), m_primitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
    initBuffers(device, vertices, indices);
}

Mesh::~Mesh()
{
    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
}

const UINT Mesh::getIndicesCount() const { return static_cast<UINT>(m_indices.size()); }

void Mesh::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology) { m_primitiveTopology = primitiveTopology; }

void Mesh::loadFromOBJ(const std::string &filepath) // right-hand, ccw => left-hand, cw
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("Mesh::loadFromOBJ: Failed to open OBJ file: " + filepath);
    }

    std::vector<DirectX::XMFLOAT3> positions = {{0.f, 0.f, 0.f}};
    std::vector<DirectX::XMFLOAT3> normals = {{0.f, 0.f, 0.f}};
    std::vector<DirectX::XMFLOAT2> texCoords = {{0.f, 0.f}};
    m_vertices.clear();
    m_indices.clear();

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v")
        {
            DirectX::XMFLOAT3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            pos.z = -pos.z;
            positions.push_back(pos);
        }
        else if (type == "vt")
        {
            DirectX::XMFLOAT2 tex;
            iss >> tex.x >> tex.y;
            tex.y = 1.f - tex.y;
            texCoords.push_back(tex);
        }
        else if (type == "vn")
        {
            DirectX::XMFLOAT3 norm;
            iss >> norm.x >> norm.y >> norm.z;
            norm.z = -norm.z;
            normals.push_back(norm);
        }
        else if (type == "f")
        {
            std::vector<DirectX::XMINT3> face_vertices;
            std::string descriptor;
            while (iss >> descriptor)
            {
                size_t firstSlash = descriptor.find('/');
                size_t secondSlash = descriptor.find('/', firstSlash + 1);

                int vIndex = std::stoi(descriptor.substr(0, firstSlash));
                int tIndex = std::stoi(descriptor.substr(firstSlash + 1, secondSlash - firstSlash - 1));
                int nIndex = std::stoi(descriptor.substr(secondSlash + 1));

                face_vertices.push_back({vIndex, tIndex, nIndex}); // v, t, n
            }

            size_t size_vertices = face_vertices.size();
            if (size_vertices == 3)
            {
                for (int i = 0; i < 3; ++i)
                {
                    auto &fv = face_vertices[i];
                    m_vertices.push_back({positions[fv.x], normals[fv.z], texCoords[fv.y]}); // v, n, t
                    m_indices.push_back(static_cast<uint32_t>(m_vertices.size() - 1));
                }
            }
            else if (size_vertices == 4)
            {
                const int A = 0, B = 1, C = 2, D = 3;
                int quad_indices[6] = {A, B, C, A, C, D};
                for (int i = 0; i < 6; ++i)
                {
                    auto &fv = face_vertices[quad_indices[i]];
                    m_vertices.push_back({positions[fv.x], normals[fv.z], texCoords[fv.y]}); // v, n, t
                    m_indices.push_back(static_cast<uint32_t>(m_vertices.size() - 1));       // TODO: 1-1 mapping, need to optimize
                }
            }
        }
    }
    file.close();
}

void Mesh::initBuffers(ID3D11Device *device, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    D3D11_SUBRESOURCE_DATA initData = {};
    HRESULT hr;

    // Vertex Buffer
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    initData.pSysMem = vertices.data();

    hr = device->CreateBuffer(&bufferDesc, &initData, m_vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Mesh::initBuffers: Failed to create vertex buffer.");
    }

    // Index Buffer
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indices.size());
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    initData.pSysMem = indices.data();

    hr = device->CreateBuffer(&bufferDesc, &initData, m_indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Mesh::initBuffers: Failed to create index buffer.");
    }
}

void Mesh::bind(ID3D11DeviceContext *deviceContext) const
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(m_primitiveTopology);
}
