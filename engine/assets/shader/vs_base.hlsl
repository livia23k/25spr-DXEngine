
struct VS_INPUT
{
    float3 pos    : POSITION;
    float3 normal : NORMAL;
    float2 uv     : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos    : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv     : TEXCOORD;
    float3 worldPos : TEXCOORD1;
};

cbuffer ModelBuffer : register(b0)
{
    matrix world;
}

cbuffer CameraBuffer : register(b1)
{
    matrix view;
    matrix projection;
}

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    float4 worldPos = mul(float4(input.pos, 1.f), world);
    float4 viewPos = mul(worldPos, view);

    output.worldPos = worldPos.xyz;
    output.pos = mul(viewPos, projection);
	output.normal = normalize(mul(input.normal, world));
    output.uv = input.uv;
    
    return output;
}