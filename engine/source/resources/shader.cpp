#include "resources/shader.h"
#include <d3dcompiler.h>
#include <assert.h>
#include <stdexcept>

Shader::Shader(ID3D11Device *device,
               const std::wstring &vertexShaderPath,
               const std::wstring &pixelShaderPath,
               const D3D11_INPUT_ELEMENT_DESC *inputElementDescs,
               UINT numInputElementDescs)
{
    // vertex shader

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
    HRESULT hr = compileShaderFromFile(vertexShaderPath, "VSMain", "vs_5_0", vsBlob.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Shader:Shader: Failed to compile vertex shader");
    }

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Shader:Shader: Failed to create vertex shader");
    }

    hr = device->CreateInputLayout(inputElementDescs, numInputElementDescs,
                                   vsBlob->GetBufferPointer(),
                                   vsBlob->GetBufferSize(),
                                   m_inputLayout.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Shader:Shader: Failed to create input layout");
    }

    // pixel shader

    Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
    hr = compileShaderFromFile(pixelShaderPath, "PSMain", "ps_5_0", psBlob.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Shader:Shader: Failed to compile pixel shader");
    }

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Shader:Shader: Failed to create pixel shader");
    }
}

void Shader::bind(ID3D11DeviceContext *deviceContext) const
{
    deviceContext->IASetInputLayout(m_inputLayout.Get());
    deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

ID3D11InputLayout *Shader::getInputLayout() const
{
    return m_inputLayout.Get();
}

HRESULT Shader::compileShaderFromFile(const std::wstring &fileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob **ppBlobOut)
{
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif

    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3DCompileFromFile(fileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel, shaderFlags, 0, ppBlobOut, errorBlob.GetAddressOf());
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA(reinterpret_cast<const char *>(errorBlob->GetBufferPointer()));
        }
    }
    return hr;
}
