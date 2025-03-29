struct PS_INPUT
{
    float4 pos    : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv     : TEXCOORD;
};

Texture2D albedoTexture : register(t2);
SamplerState albedoSampler : register(s2);

cbuffer MaterialBuffer : register(b2)
{
    float4 albedo;
    int useTexture;
    int padding[3];
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 color = (useTexture != 0) ? albedoTexture.Sample(albedoSampler, input.uv) : albedo;
    return float4(color.rgb, 1.0f);
}