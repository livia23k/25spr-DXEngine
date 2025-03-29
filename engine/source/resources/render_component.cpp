#include "resources/render_component.h"

RenderComponent::RenderComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<MaterialBase> material)
    : m_isCullFront(true), m_mesh(mesh), m_material(material) {}

void RenderComponent::render(ID3D11DeviceContext *deviceContext)
{
    if (m_mesh)
    {
        m_mesh->bind(deviceContext);
    }
    if (m_material)
    {
        m_material->bind(deviceContext);
    }
    deviceContext->DrawIndexed(m_mesh->getIndicesCount(), 0, 0); // TODO: index offset if needed
}
