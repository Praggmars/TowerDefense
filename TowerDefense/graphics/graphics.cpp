#include "pch.h"
#include "graphics.h"
#include "common/helpers.h"
#include <windows.ui.xaml.media.dxinterop.h>

namespace TowerDefense
{
	namespace gfx
	{
		void Graphics::CreateDevice()
		{
			D3D_FEATURE_LEVEL featureLevels[] =
			{
				D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
			};
			Microsoft::WRL::ComPtr<ID3D11Device> device;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

			HRESULT hr = D3D11CreateDevice(
				nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevels,
				ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &m_featureLevel, &context);

			if (FAILED(hr))
				ThrowIfFailed(D3D11CreateDevice(
					nullptr, D3D_DRIVER_TYPE_WARP, 0, D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevels,
					ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &m_featureLevel, &context));

			ThrowIfFailed(device.As(&m_device3D));
			ThrowIfFailed(context.As(&m_context3D));
		}
		void Graphics::CreateRasterizerState()
		{
			D3D11_RASTERIZER_DESC2 rasterizerDesc{};
			rasterizerDesc.AntialiasedLineEnable = true;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0.0f;
			rasterizerDesc.SlopeScaledDepthBias = 0.0f;
			rasterizerDesc.DepthClipEnable = true;
			rasterizerDesc.FrontCounterClockwise = false;
			rasterizerDesc.MultisampleEnable = true;
			rasterizerDesc.ScissorEnable = false;
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.ForcedSampleCount = 0;
			rasterizerDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF;
			ThrowIfFailed(m_device3D->CreateRasterizerState2(&rasterizerDesc, &m_rasterizerState));
		}
		void Graphics::CreateDepthStencilState()
		{
			D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			depthStencilDesc.StencilEnable = true;
			depthStencilDesc.StencilReadMask = 0xFF;
			depthStencilDesc.StencilWriteMask = 0xFF;
			depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			ThrowIfFailed(m_device3D->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState));
			m_context3D->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
		}
		void Graphics::CreateBlendState()
		{
			HRESULT result;
			D3D11_BLEND_DESC blendDesc{};
			blendDesc.RenderTarget[0].BlendEnable = false;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

			blendDesc.RenderTarget[0].BlendEnable = true;
			result = m_device3D->CreateBlendState(&blendDesc, &m_blendState);
			if (FAILED(result))
				throw std::exception("Failed to create blend state");

			float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			//m_context3D->OMSetBlendState(m_blendState.Get(), blendFactor, 0xffffffff);
		}
		void Graphics::CreateWindowSizeDependentResources()
		{
			ID3D11RenderTargetView* nullViews[] = { nullptr };
			m_context3D->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
			m_renderTargetView = nullptr;
			m_depthStencilView = nullptr;
			m_context3D->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

			CreateSwapChain();
			CreateRenderTargetView();
			CreateViewport();

			m_swapChainPanel->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::High, ref new Windows::UI::Core::DispatchedHandler([=]()
				{
					Microsoft::WRL::ComPtr<ISwapChainPanelNative> panelNative;
					ThrowIfFailed(reinterpret_cast<IUnknown*>(m_swapChainPanel)->QueryInterface(IID_PPV_ARGS(&panelNative)));
					ThrowIfFailed(panelNative->SetSwapChain(m_swapChain.Get()));
				}, Platform::CallbackContext::Any));
		}
		void Graphics::CreateSwapChain()
		{
			if (m_swapChain)
			{
				ThrowIfFailed(m_swapChain->ResizeBuffers(2, m_width, m_height, DXGI_FORMAT_B8G8R8A8_UNORM, 0));
			}
			else
			{
				DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
				swapChainDesc.Width = m_width;
				swapChainDesc.Height = m_height;
				swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				swapChainDesc.Stereo = false;
				swapChainDesc.SampleDesc.Count = 1;
				swapChainDesc.SampleDesc.Quality = 0;
				swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				swapChainDesc.BufferCount = 2;
				swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
				swapChainDesc.Flags = 0;
				swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
				swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

				Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
				ThrowIfFailed(m_device3D.As(&dxgiDevice));

				Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
				ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

				Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
				ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

				Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
				ThrowIfFailed(dxgiFactory->CreateSwapChainForComposition(m_device3D.Get(), &swapChainDesc, nullptr, &swapChain));

				ThrowIfFailed(swapChain.As(&m_swapChain));

				ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));
			}
		}
		void Graphics::CreateRenderTargetView()
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D1> backBuffer;
			ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
			ThrowIfFailed(m_device3D->CreateRenderTargetView1(backBuffer.Get(), nullptr, &m_renderTargetView));

			CD3D11_TEXTURE2D_DESC1 depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, m_width, m_height, 1, 1, D3D11_BIND_DEPTH_STENCIL);
			Microsoft::WRL::ComPtr<ID3D11Texture2D1> depthStencil;
			ThrowIfFailed(m_device3D->CreateTexture2D1(&depthStencilDesc, nullptr, &depthStencil));
			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
			ThrowIfFailed(m_device3D->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &m_depthStencilView));
		}
		void Graphics::CreateViewport()
		{
			m_viewport = CD3D11_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height));
		}
		void Graphics::CreateVertexShader()
		{
			static const char* code = R"(
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix cameraMatrix;
	matrix lightMatrix;
};

cbuffer BoneBuffer
{
	matrix bones[96];
};

struct VertexInputType
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float3 weights  : WEIGHTS;
	uint4 bones     : BONES;
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

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	output.wndpos = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = output.wndpos.xyz;
	output.lightTex = mul(output.wndpos, lightMatrix);
	output.wndpos = mul(output.wndpos, cameraMatrix);
	output.texcoord = input.texcoord;
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	return output;
}
)";
			const char* target = nullptr;
			switch (m_featureLevel)
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
			Microsoft::WRL::ComPtr<ID3DBlob>byteCode = CompileShader(code, "main", target);
			ThrowIfFailed(m_device3D->CreateVertexShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &m_vertexShader));
			D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[6];
			inputLayoutDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,      0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[2] = { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[3] = { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[4] = { "WEIGHTS",  0, DXGI_FORMAT_R32G32B32_FLOAT,   0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			inputLayoutDesc[5] = { "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			ThrowIfFailed(m_device3D->CreateInputLayout(inputLayoutDesc, 6, byteCode->GetBufferPointer(), byteCode->GetBufferSize(), &m_inputLayout));

			m_context3D->IASetInputLayout(m_inputLayout.Get());
		}
		void Graphics::CreatePixelShader()
		{
			static const char* code = R"(
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
	float4 color = light_ambientColor * occlusionFactor;

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
			const char* target = nullptr;
			switch (m_featureLevel)
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
			ThrowIfFailed(m_device3D->CreatePixelShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &m_pixelShader));
		}
		void Graphics::CreateShaderBuffers()
		{
			D3D11_BUFFER_DESC bufferDesc;
			HRESULT hr;

			m_vsMatrixBufferSize = sizeof(CB_MatrixBuffer);
			m_psLightBufferSize = sizeof(CB_LightBuffer);
			m_psMaterialBufferSize = sizeof(CB_MaterialBuffer);

			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = 0;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			bufferDesc.ByteWidth = m_vsMatrixBufferSize;
			ThrowIfFailed(m_device3D->CreateBuffer(&bufferDesc, nullptr, &m_vsMatrixBuffer));

			bufferDesc.ByteWidth = m_psLightBufferSize;
			ThrowIfFailed(m_device3D->CreateBuffer(&bufferDesc, nullptr, &m_psLightBuffer));

			bufferDesc.ByteWidth = m_psMaterialBufferSize;
			ThrowIfFailed(m_device3D->CreateBuffer(&bufferDesc, nullptr, &m_psMaterialBuffer));
		}
		void Graphics::WriteShaderBuffer(ID3D11Buffer* buffer, void* data, unsigned size)
		{
			D3D11_MAPPED_SUBRESOURCE resource;
			if (SUCCEEDED(m_context3D->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
			{
				memcpy(resource.pData, data, size);
				m_context3D->Unmap(buffer, 0);
			}
		}
		void Graphics::CreateSamplerStates()
		{
			D3D11_SAMPLER_DESC samplerDesc{};
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
			samplerDesc.BorderColor[0] = 1.0f;
			samplerDesc.BorderColor[1] = 1.0f;
			samplerDesc.BorderColor[2] = 1.0f;
			samplerDesc.BorderColor[3] = 1.0f;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
			ThrowIfFailed(m_device3D->CreateSamplerState(&samplerDesc, &m_textureSampler));

			samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			ThrowIfFailed(m_device3D->CreateSamplerState(&samplerDesc, &m_shadowSampler));
		}
		Graphics::Graphics() :
			m_viewport(),
			m_featureLevel(D3D_FEATURE_LEVEL_9_1),
			m_width(0),
			m_height(0)
		{
			CreateDevice();
			CreateRasterizerState();
			CreateDepthStencilState();
			CreateBlendState();

			CreateVertexShader();
			CreatePixelShader();
			CreateShaderBuffers();
			CreateSamplerStates();

			m_context3D->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		Graphics::P Graphics::CreateP()
		{
			return std::make_shared<Graphics>();
		}
		Graphics::U Graphics::CreateU()
		{
			return std::make_unique<Graphics>();
		}
		void Graphics::Clear()
		{
			float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			Clear(color);
		}
		void Graphics::Clear(float r, float g, float b, float a)
		{
			float color[] = { r, g, b, a };
			Clear(color);
		}
		void Graphics::Clear(float color[4])
		{
			m_context3D->ClearRenderTargetView(m_renderTargetView.Get(), color);
			m_context3D->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
		void Graphics::SetScreenAsRenderTarget()
		{
			ID3D11RenderTargetView* rendeTargets[1] = { m_renderTargetView.Get() };
			m_context3D->RSSetState(m_rasterizerState.Get());
			m_context3D->VSSetShader(m_vertexShader.Get(), nullptr, 0);
			m_context3D->PSSetShader(m_pixelShader.Get(), nullptr, 0);
			//m_context3D->IASetInputLayout(m_inputLayout.Get());
			m_context3D->RSSetViewports(1, &m_viewport);
			m_context3D->OMSetRenderTargets(1, rendeTargets, m_depthStencilView.Get());
			m_context3D->PSSetSamplers(0, 1, m_textureSampler.GetAddressOf());
			m_context3D->PSSetSamplers(1, 1, m_shadowSampler.GetAddressOf());
			m_context3D->VSSetConstantBuffers(0, 1, m_vsMatrixBuffer.GetAddressOf());
			m_context3D->PSSetConstantBuffers(0, 1, m_psLightBuffer.GetAddressOf());
			m_context3D->PSSetConstantBuffers(1, 1, m_psMaterialBuffer.GetAddressOf());
		}
		void Graphics::Present()
		{
			m_swapChain->Present(1, 0);
			/*DXGI_PRESENT_PARAMETERS parameters = { 0 };
			HRESULT hr = m_swapChain->Present1(1, 0, &parameters);
			m_context3D->DiscardView1(m_renderTargetView.Get(), nullptr, 0);
			m_context3D->DiscardView1(m_depthStencilView.Get(), nullptr, 0);*/
		}
		void Graphics::WriteVSMatrixBuffer(void* data)
		{
			WriteShaderBuffer(m_vsMatrixBuffer.Get(), data, m_vsMatrixBufferSize);
		}
		void Graphics::WritePSLightBuffer(void* data)
		{
			WriteShaderBuffer(m_psLightBuffer.Get(), data, m_psLightBufferSize);
		}
		void Graphics::WritePSMaterialBuffer(void* data)
		{
			WriteShaderBuffer(m_psMaterialBuffer.Get(), data, m_psMaterialBufferSize);
		}
		void Graphics::SetSwapChainPanel(Windows::UI::Xaml::Controls::SwapChainPanel^ panel)
		{
			m_swapChainPanel = panel;
			m_width = max(1, static_cast<unsigned>(panel->Width));
			m_height = max(1, static_cast<unsigned>(panel->Height));
			CreateWindowSizeDependentResources();
		}
		void Graphics::Resize(unsigned width, unsigned height)
		{
			m_width = width;
			m_height = height;
			CreateWindowSizeDependentResources();
		}
	}
}