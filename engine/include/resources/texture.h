#pragma once

#include "utils/forward.h"

class TextureBase {
public:
    virtual ~TextureBase() = default;
    
    virtual void bind(ID3D11DeviceContext* deviceContext, UINT slot) const = 0;

protected:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
};

class ConstantTexture : public TextureBase {
public:
    ConstantTexture(ID3D11Device* device, const DirectX::XMFLOAT4& color);
    ~ConstantTexture() = default;

    void bind(ID3D11DeviceContext* deviceContext, UINT slot) const override;

private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
};

class ImageTexture : public TextureBase {
public:
    ImageTexture(ID3D11Device* device, const std::string& filePath);
    ~ImageTexture() = default;

    void bind(ID3D11DeviceContext* deviceContext, UINT slot) const override;

private:
    void loadFromFile(ID3D11Device* device, const std::string& filePath);
    
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
};
