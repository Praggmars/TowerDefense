Texture2D tex;
Texture2D normalmap;
SamplerState ss;

cbuffer LightBuffer
{
	float4 lightColor;
	float3 lightPosition;
	float ambient;
};

struct PixelInputType
{
	float4 wndpos : SV_POSITION;
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 color = tex.Sample(ss, input.texcoord);
	float4 bumpmap = normalmap.Sample(ss, input.texcoord) * 2.0f - 1.0f;

	input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);
	input.normal = normalize(input.normal);
	float3 bitangent = cross(input.tangent, input.normal);
	float3x3 texSpace = float3x3(input.tangent, bitangent, input.normal);
	float3 normal = normalize(mul(bumpmap, texSpace));

	float3 lightDirection = normalize(lightPosition - input.position);
	float intensity = saturate(dot(normal, lightDirection));
	intensity = ambient + (1 - ambient) * intensity;
	color.xyz *= intensity;

	return color * lightColor;
}