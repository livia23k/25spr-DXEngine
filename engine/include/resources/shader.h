#pragma once

#include "utils/forward.h"

class Shader
{
public:
    Shader(ID3D11Device *device, const std::wstring &vertexShaderPath, const std::wstring &pixelShaderPath, const D3D11_INPUT_ELEMENT_DESC *inputElementDescs, UINT numInputElementDescs);
    ~Shader() = default;

    void bind(ID3D11DeviceContext *deviceContext) const;

    ID3D11InputLayout *getInputLayout() const;

private:
    HRESULT compileShaderFromFile(const std::wstring &fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob **ppBlobOut);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};
