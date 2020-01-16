#include "pch.h"
#include "vertexshader.h"

namespace TowerDefense
{
	namespace gfx
	{
		static const char* g_ShaderCode = R"(
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix cameraMatrix;
};

struct VertexInputType
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct PixelInputType
{
	float4 wndpos: SV_POSITION;
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	output.wndpos = mul(float4(input.position, 1.0f), worldMatrix); 
	output.position = output.wndpos.xyz;
	output.wndpos = mul(output.wndpos, cameraMatrix);
	output.texcoord = input.texcoord;
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	return output;
}
)";

		VertexShader::VertexShader(Graphics& graphics)
		{
			const char* target = nullptr;
			switch (graphics.FeatureLevel())
			{
			case D3D_FEATURE_LEVEL_10_0:
				target = "vs_4_0";
				break;
			case D3D_FEATURE_LEVEL_10_1:
				target = "vs_4_1";
				break;
			case D3D_FEATURE_LEVEL_11_0:
			case D3D_FEATURE_LEVEL_11_1:
			case D3D_FEATURE_LEVEL_12_0:
			case D3D_FEATURE_LEVEL_12_1:
				target = "vs_5_0";
				break;
			default:
				target = "vs_3_0";
			}
			Microsoft::WRL::ComPtr<ID3DBlob> shaderBuffer = CompileShader(g_ShaderCode, "main", target);

			ThrowIfFailed(graphics.Device3D()->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, &m_vertexShader));
			D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[4];
			inputLayoutDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

			ThrowIfFailed(graphics.Device3D()->CreateInputLayout(
				inputLayoutDesc, 4, shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &m_inputLayout));
		}
		VertexShader::P VertexShader::CreateP(Graphics& graphics)
		{
			return std::make_shared<VertexShader>(graphics);
		}
		VertexShader::U VertexShader::CreateU(Graphics& graphics)
		{
			return std::make_unique<VertexShader>(graphics);
		}
		void VertexShader::SetToRender(Graphics& graphics)
		{
			ID3D11DeviceContext3* context = graphics.Context3D();
			context->IASetInputLayout(m_inputLayout.Get());
			context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
		}
	}
}