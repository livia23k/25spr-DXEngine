#pragma once

#include "utils/forward.h"
#include "resources/mesh.h"
#include "resources/material.h"

class RenderComponent
{
public:
    RenderComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<MaterialBase> material);

    void setIsCullFront(bool isCullFront) { m_isCullFront = isCullFront; }
    bool getIsCullFront() const { return m_isCullFront; }

    void render(ID3D11DeviceContext *deviceContext);

private:
    bool m_isCullFront;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<MaterialBase> m_material;
};
