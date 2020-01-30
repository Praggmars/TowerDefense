#include "pch.h"
#include "shadowmap.h"

namespace Converter
{
	namespace gfx
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> ShadowMap::CreateDepthMap(ID3D11Device3* device)
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = m_width;
			textureDesc.Height = m_height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;
			Microsoft::WRL::ComPtr<ID3D11Texture2D> depthMap;
			ThrowIfFailed(device->CreateTexture2D(&textureDesc, nullptr, &depthMap));
			return depthMap;
		}
		void ShadowMap::CreateRasterizerState(ID3D11Device3* device)
		{
			D3D11_RASTERIZER_DESC2 rasterizerDesc{};
			rasterizerDesc.AntialiasedLineEnable = true;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.DepthBias = 3000;
			rasterizerDesc.DepthBiasClamp = 0.0f;
			rasterizerDesc.SlopeScaledDepthBias = 1.0f;
			rasterizerDesc.DepthClipEnable = false;
			rasterizerDesc.FrontCounterClockwise = false;
			rasterizerDesc.MultisampleEnable = true;
			rasterizerDesc.ScissorEnable = false;
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.ForcedSampleCount = 0;
			rasterizerDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF;
			ThrowIfFailed(device->CreateRasterizerState2(&rasterizerDesc, &m_rasterizerState));
		}
		void ShadowMap::CreateVertexShader(Graphics& graphics)
		{
			static const char* code = R"(
cbuffer MatrixBuffer
{
	matrix lightMatrix;
};

struct VertexInputType
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
};

struct PixelInputType
{
	float4 wndpos   : SV_POSITION;
	//float2 texcoord : TEXCOORD;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	output.wndpos = mul(float4(input.position, 1.0f), lightMatrix);
	//output.texcoord = input.texcoord;
	return output;
}
)";
			auto* device = graphics.Device3D();
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
				target = "vs_5_0";
				break;
			default:
				target = "vs_4_0";
			}
			Microsoft::WRL::ComPtr<ID3DBlob> byteCode = CompileShader(code, "main", target);
			ThrowIfFailed(device->CreateVertexShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &m_vertexShader));
			D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[4];
			inputLayoutDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			ThrowIfFailed(device->CreateInputLayout(inputLayoutDesc, 4, byteCode->GetBufferPointer(), byteCode->GetBufferSize(), &m_inputLayout));
		}
		void ShadowMap::CreatePixelShader(Graphics& graphics)
		{
			static const char* code = R"(
Texture2D texture_diffuse;
SamplerState textureSampler;

struct PixelInputType
{
	float4 wndpos   : SV_POSITION;
	//float2 texcoord : TEXCOORD;
};

void main(PixelInputType input)
{
	//float4 color = texture_diffuse.Sample(textureSampler, input.texcoord);
	//clip(color.a - 0.15f);
}
)";
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
				target = "ps_5_0";
				break;
			default:
				target = "ps_4_0";
			}
			Microsoft::WRL::ComPtr<ID3DBlob> byteCode = CompileShader(code, "main", target);
			ThrowIfFailed(graphics.Device3D()->CreatePixelShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &m_pixelShader));
		}
		void ShadowMap::CreateBuffer(ID3D11Device3* device)
		{
			D3D11_BUFFER_DESC bufferDesc;

			m_bufferSize = sizeof(mth::float4x4);

			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = 0;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.ByteWidth = m_bufferSize;
			ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, &m_buffer));
		}
		ShadowMap::ShadowMap(Graphics& graphics, unsigned width, unsigned height) :
			m_viewport{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f },
			m_width(width),
			m_height(height)
		{
			auto* device = graphics.Device3D();
			auto depthMap = CreateDepthMap(device);

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Flags = 0;
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			ThrowIfFailed(device->CreateDepthStencilView(depthMap.Get(), &dsvDesc, &m_depthStencilView));

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
			ThrowIfFailed(device->CreateShaderResourceView(depthMap.Get(), &srvDesc, &m_shaderResourceView));

			CreateRasterizerState(device);
			CreateVertexShader(graphics);
			CreatePixelShader(graphics);
			CreateBuffer(device);
		}
		ShadowMap::P ShadowMap::CreateP(Graphics& graphics, unsigned width, unsigned height)
		{
			return std::make_shared<ShadowMap>(graphics, width, height);
		}
		ShadowMap::U ShadowMap::CreateU(Graphics& graphics, unsigned width, unsigned height)
		{
			return std::make_unique<ShadowMap>(graphics, width, height);
		}
		void ShadowMap::SetTextureToRender(Graphics& graphics, unsigned index)
		{
			graphics.Context3D()->PSSetShaderResources(index, 1, m_shaderResourceView.GetAddressOf());
		}
		void ShadowMap::SetAsRenderTarget(Graphics& graphics)
		{
			auto* context = graphics.Context3D();
			ID3D11RenderTargetView* renderTargets[1] = { nullptr };
			context->RSSetState(m_rasterizerState.Get());
			context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
			//context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
			context->PSSetShader(nullptr, nullptr, 0);
			//context->PSSetSamplers(1, 1, m_samplerState.GetAddressOf());
			context->IASetInputLayout(m_inputLayout.Get());
			context->RSSetViewports(1, &m_viewport);
			context->OMSetRenderTargets(1, renderTargets, m_depthStencilView.Get());
			context->VSSetConstantBuffers(0, 1, m_buffer.GetAddressOf());
		}
		void ShadowMap::WriteBuffer(Graphics& graphics, void* data)
		{
			D3D11_MAPPED_SUBRESOURCE resource;
			auto* context = graphics.Context3D();
			if (SUCCEEDED(context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
			{
				memcpy(resource.pData, data, m_bufferSize);
				context->Unmap(m_buffer.Get(), 0);
			}
		}
	}
}