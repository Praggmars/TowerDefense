#include "pch.h"
#include "pixelshader.h"

namespace TowerDefense
{
	namespace gfx
	{
		static const char* g_ShaderCode = R"(
Texture2D tex;
Texture2D normalmap;
SamplerState ss;

cbuffer LightBuffer
{
	float4 lightColor;
	float3 lightPosition;
	float ambient;
};

cbuffer MaterialBuffer
{
	float4 materialColor;
	float textureWeight;
	float padding[3];
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
	color = color * textureWeight + materialColor * (1.0f - textureWeight);
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