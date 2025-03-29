#pragma once

#include "utils/forward.h"
#include "resources/shader.h"
#include "resources/texture.h"
#include "resources/buffer_type.h"
#include <string>

class MaterialBase
{
public:
    virtual ~MaterialBase() = default;

    virtual void bind(ID3D11DeviceContext *deviceContext) const = 0;
    virtual void setProperty(const std::string &name, const void *data, size_t size) = 0;
};

class LambertianMaterial : public MaterialBase
{
public:
    LambertianMaterial(ID3D11Device *device, std::shared_ptr<Shader> shader, const DirectX::XMFLOAT4 &albedo);
    LambertianMaterial(ID3D11Device *device, std::shared_ptr<Shader> shader, const std::string &albedoPath);

    void bind(ID3D11DeviceContext *deviceContext) const override;
    void setProperty(const std::string &name, const void *data, size_t size) override;

private:
    std::shared_ptr<Shader> m_shader;

    std::shared_ptr<TextureBase> m_albedoTexture;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
};
