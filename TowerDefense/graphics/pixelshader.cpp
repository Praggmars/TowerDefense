#include "pch.h"
#include "pixelshader.h"

namespace TowerDefense
{
	namespace gfx
	{
		static const char* g_ShaderCode = R"(
Texture2D texture_diffuse : register(t0);
Texture2D texture_normal : register(t1);
Texture2D texture_shadowMap : register(t2);
Texture2D texture_ambientMap : register(t3);
SamplerState sampler_texture : register(s0);
SamplerComparisonState sampler_shadowMap : register(s1);

cbuffer LightBuffer
{
	float4 light_diffuseColor;
	float4 light_ambientColor;
	float3 light_sourcePosition;
	float  light_shadowMapDelta;
	float3 light_eyePosition;
	float  light_padding1;
	float2 light_screenSize;
	float  light_padding2[2];
};

cbuffer MaterialBuffer
{
	float4 material_diffuseColor;
	float4 material_specularColor;
	float  material_textureWeight;
	float  material_specularPower;
	float  material_padding[2];
};

struct PixelInputType
{
	float4 wndpos   : SV_POSITION;
	float3 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
    float4 lightTex : TEXCOORD1;
};

float4 PixelColor(float2 texCoord)
{
	return texture_diffuse.Sample(sampler_texture, texCoord) * material_textureWeight + material_diffuseColor * (1.0f - material_textureWeight);
}
float3 PixelNormal(float3 normal, float3 tangent, float2 texCoord)
{
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	normal = normalize(normal);
	float3 bitangent = cross(tangent, normal);
	float4 bumpmap = texture_normal.Sample(sampler_texture, texCoord) * 2.0f - 1.0f;
	return normalize(mul(bumpmap.xyz, float3x3(tangent, bitangent, normal)));
}
float ShadowFactor1x1(float3 lightTex)
{
	return texture_shadowMap.SampleCmpLevelZero(sampler_shadowMap, lightTex.xy, lightTex.z).r;
}
float ShadowFactor3x3(float3 lightTex)
{
	const float d = light_shadowMapDelta;
	float percentLit = 0.0f;
	const float2 offsets[] = {
		float2(-d,   -d), float2(0.0f,   -d), float2(d,   -d),
		float2(-d, 0.0f), float2(0.0f, 0.0f), float2(d, 0.0f),
		float2(-d,    d), float2(0.0f,    d), float2(d,    d)
	};
	[unroll]
	for (int i = 0; i < 9; i++)
		percentLit += texture_shadowMap.SampleCmpLevelZero(sampler_shadowMap, lightTex.xy + offsets[i], lightTex.z).r;
	return percentLit / 9.0f;
}
float ShadowFactor5x5(float3 lightTex)
{
	const float d = light_shadowMapDelta;
	float percentLit = 0.0f;
	const float2 offsets[] = {
		float2(-2.0f * d, -2.0f * d), float2(-d, -2.0f * d), float2(0.0f, -2.0f * d), float2(d, -2.0f * d), float2(2.0f * d, -2.0f * d),
		float2(-2.0f * d,        -d), float2(-d,        -d), float2(0.0f,        -d), float2(d,        -d), float2(2.0f * d,        -d),
		float2(-2.0f * d,      0.0f), float2(-d,      0.0f), float2(0.0f,      0.0f), float2(d,      0.0f), float2(2.0f * d,      0.0f),
		float2(-2.0f * d,         d), float2(-d,         d), float2(0.0f,         d), float2(d,         d), float2(2.0f * d,         d),
		float2(-2.0f * d,  2.0f * d), float2(-d,  2.0f * d), float2(0.0f,  2.0f * d), float2(d,  2.0f * d), float2(2.0f * d,  2.0f * d)
	};
	[unroll]
	for (int i = 0; i < 25; i++)
		percentLit += texture_shadowMap.SampleCmpLevelZero(sampler_shadowMap, lightTex.xy + offsets[i], lightTex.z).r;
	return percentLit / 25.0f;
}

float4 main(PixelInputType input) : SV_TARGET
{
	float2 texCoords = input.wndpos.xy / light_screenSize;
	//return texture_ambientMap.Sample(sampler_texture, texCoords);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 pixelColor = PixelColor(input.texcoord);
	float3 pixelNormal = PixelNormal(input.normal, input.tangent, input.texcoord);

	float3 viewDirection = normalize(light_eyePosition - input.position);
	float3 lightDirection = normalize(light_sourcePosition - input.position);

	input.lightTex.x = input.lightTex.x / input.lightTex.w * 0.5f + 0.5f;
	input.lightTex.y = input.lightTex.y / input.lightTex.w * (-0.5f) + 0.5f;
	input.lightTex.z = input.lightTex.z / input.lightTex.w;
	float shadowFactor = ShadowFactor5x5(input.lightTex.xyz);
	float intensity = shadowFactor * saturate(dot(pixelNormal, lightDirection));

	float occlusionFactor = texture_ambientMap.Sample(sampler_texture, texCoords).r;
	float4 color = light_ambientColor * (occlusionFactor * 0.5f + 0.5f);

	if (intensity > 0.0f)
	{
		color = saturate(color + light_diffuseColor * intensity);
		float3 reflection = normalize(2.0f * intensity * pixelNormal - lightDirection);
		reflection = reflect(-lightDirection, pixelNormal);
		specular = pow(saturate(dot(reflection, viewDirection)), material_specularPower);
		specular *= material_specularColor;
	}
	color = color * pixelColor;

	color = saturate(color + specular);
	color.w = pixelColor.w;
	return color;
}
)";

		PixelShader::PixelShader(Graphics& graphics)
		{
			const char* target = nullptr;
			switch (graphics.FeatureLevel())
			{
			case D3D_FEATURE_LEVEL_10_0:
				target = "ps_4_0";
				break;
			case D3D_FEATURE_LEVEL_10_1:
				target = "ps_4_1";
				break;
			case D3D_FEATURE_LEVEL_11_0:
			case D3D_FEATURE_LEVEL_11_1:
			case D3D_FEATURE_LEVEL_12_0:
			case D3D_FEATURE_LEVEL_12_1:
				target = "ps_5_0";
				break;
			default:
				target = "ps_3_0";
			}
			Microsoft::WRL::ComPtr<ID3DBlob> shaderBuffer = CompileShader(g_ShaderCode, "main", target);
			ThrowIfFailed(graphics.Device3D()->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, &m_pixelShader));
		}
		PixelShader::P PixelShader::CreateP(Graphics& graphics)
		{
			return std::make_shared<PixelShader>(graphics);
		}
		PixelShader::U PixelShader::CreateU(Graphics& graphics)
		{
			return std::make_unique<PixelShader>(graphics);
		}
		void PixelShader::SetToRender(Graphics& graphics)
		{
			graphics.Context3D()->PSSetShader(m_pixelShader.Get(), nullptr, 0);
		}
	}
}