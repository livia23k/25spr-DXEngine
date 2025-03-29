struct PS_INPUT
{
    float4 pos    : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv     : TEXCOORD;
    float3 worldPos : TEXCOORD1;
};

Texture2D albedoTexture : register(t2);
SamplerState albedoSampler : register(s2);

cbuffer MaterialBuffer : register(b2)
{
    float4 albedo;
    int useTexture;
    int padding[3];
};

struct Light {
    float4 position; // .xyz = position, .w = type
    float4 color;    // .xyz = color, .w = intensity
};

#define MAX_LIGHTS 1

cbuffer LightArrayBuffer : register(b3)
{
    Light lights[MAX_LIGHTS];
    int numLights;
};


float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 baseColor = (useTexture != 0)
        ? albedoTexture.Sample(albedoSampler, input.uv)
        : albedo;

    float3 N = normalize(input.normal);
    float3 result = float3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        if (i >= numLights) break;

        Light light = lights[i];
        float4 lightPos = light.position;
        float4 lightCol = light.color;

        
        float dist = length(input.worldPos - lightPos.xyz); // radius of light body
        if (dist < 50.f) {
            return baseColor;
        }

        int lightType = (int)lightPos.w;

        if (lightType == 0) // Point Light
        {
            float3 L = normalize(lightPos.xyz - input.worldPos);
            float diff = dot(N, L);
            diff = max(diff, 0.2f);
            result += baseColor.rgb * lightCol.rgb * diff * lightCol.a;
        }
    }

    return float4(saturate(result), baseColor.a);
}