#include "pch.h"
#include "ambientocclusion.h"

namespace TowerDefense
{
	namespace gfx
	{
		void AmbientOcclusion::CreateNormalDepthMapTextureResources(ID3D11Device3* device)
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = m_width;
			textureDesc.Height = m_height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;
			ThrowIfFailed(device->CreateTexture2D(&textureDesc, nullptr, &m_normalDepthMapTex));

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
			ThrowIfFailed(device->CreateShaderResourceView(m_normalDepthMapTex.Get(), &srvDesc, &m_normalDepthMapSRV));

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = textureDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			ThrowIfFailed(device->CreateRenderTargetView(m_normalDepthMapTex.Get(), &rtvDesc, &m_normalDepthMapRTV));
		}
		void AmbientOcclusion::CreateNormalDepthMapVertexShader(Graphics& graphics)
		{
			static const char* code = R"(
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
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
	float  depth    : DEPTH;
	float3 normal   : NORMAL;
	//float2 texcoord : TEXCOORD;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	matrix transformMatrix = mul(
		input.weights.x * bones[input.bones.x] + 
		input.weights.y * bones[input.bones.y] + 
		input.weights.z * bones[input.bones.z] +
		(1.0f - input.weights.x - input.weights.y - input.weights.z) * bones[input.bones.w],
		worldMatrix);
	output.wndpos = mul(float4(input.position, 1.0f), transformMatrix);
	output.depth = mul(output.wndpos, viewMatrix).z;
	output.normal = mul(input.normal, (float3x3)transformMatrix);
	output.wndpos = mul(output.wndpos, cameraMatrix);
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
			ThrowIfFailed(device->CreateVertexShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &m_vsNormalDepthMap));
		}
		void AmbientOcclusion::CreateNormalDepthMapPixelShader(Graphics& graphics)
		{
			static const char* code = R"(
//Texture2D texture_diffuse;
//SamplerState textureSampler;

struct PixelInputType
{
	float4 wndpos   : SV_POSITION;
	float  depth     : DEPTH;
	float3 normal   : NORMAL;
	//float2 texcoord : TEXCOORD;
};

float4 main(PixelInputType input) : SV_TARGET
{
	//float4 color = texture_diffuse.Sample(textureSampler, input.texcoord);
	//clip(color.a - 0.15f);
	return float4(normalize(input.normal), input.depth);
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
			ThrowIfFailed(graphics.Device3D()->CreatePixelShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &m_psNormalDepthMap));
		}

		void AmbientOcclusion::CreateOcclusionMapTextureResources(ID3D11Device3* device)
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = m_width;
			textureDesc.Height = m_height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;
			ThrowIfFailed(device->CreateTexture2D(&textureDesc, nullptr, &m_occlusionTex));

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
			ThrowIfFailed(device->CreateShaderResourceView(m_occlusionTex.Get(), &srvDesc, &m_occlusionSRV));

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = textureDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			ThrowIfFailed(device->CreateRenderTargetView(m_occlusionTex.Get(), &rtvDesc, &m_occlusionRTV));
		}
		void AmbientOcclusion::CreateOcclusionMapPixelShader(Graphics& graphics)
		{
			static const char* code = R"(
Texture2D texture_normalDepthMap : register(t0);
Texture2D texture_randomVectors  : register(t1);
SamplerState wrapSampler         : register(s0);
SamplerState clampSampler        : register(s1);

cbuffer OcclusionData
{
	float4x4 viewToTexSpace;
	float4 offsetVectors[14];
	float2 screenSize;
	float2 randVecTexSize;
	float occlusionRadius;
	float occlusionFadeStart;
	float occlusionFadeEnd;
	float surfaceEpsilon;
};

struct PixelInputType
{
	float4 wndpos   : SV_POSITION;
	float3 position : POSITION;
};

float OcclusionFunction(float distZ)
{
	float occlusion = 0.0f;
	if (distZ > surfaceEpsilon)
	{
		float fadeLength = occlusionFadeEnd - occlusionFadeStart;
		occlusion = saturate((occlusionFadeEnd - distZ) / fadeLength);
	}
	return occlusion;
}

float4 main(PixelInputType input) : SV_TARGET
{
	//return float4(input.position.xyz, 1.0f);
	//clip(color.a - 0.15f);
	float2 texCoords = input.wndpos.xy / screenSize;
	float4 normalDepth = texture_normalDepthMap.Sample(clampSampler, texCoords);
	float3 position = input.position * normalDepth.w;
	float3 randVec = texture_randomVectors.Sample(wrapSampler, input.wndpos.xy / randVecTexSize);
	float occlusionSum = 0.0f;
	[unroll]
	for (int i = 0; i < 14; i++)
	{
		float3 offset = reflect(offsetVectors[i].xyz, randVec);
		float flip = sign(dot(offset, normalDepth.xyz));
		float3 q = position + flip * occlusionRadius * offset;
		float4 qProj = mul(float4(q, 1.0f), viewToTexSpace);
		qProj /= qProj.w;
		qProj.x = qProj.x * 0.5f + 0.5f;
		qProj.y = qProj.y * (-0.5f) + 0.5f;
		float rz = texture_normalDepthMap.Sample(clampSampler, qProj.xy).a;
		float3 r = (rz / q.z) * q;
		float distZ = position.z - r.z;
		float dp = max(dot(normalDepth.xyz, normalize(r - position)), 0.0f);
		float occlusion = dp * OcclusionFunction(distZ);
		occlusionSum += occlusion;
	}
	occlusionSum /= 14.0f;
	float access = saturate(1.0f - occlusionSum);
	return saturate(pow(access, 4.0f));
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
			ThrowIfFailed(graphics.Device3D()->CreatePixelShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &m_psOcclusionMap));
		}
		void AmbientOcclusion::FillOcclusionMapPSBuffer()
		{
			m_occlusionMapPSBufferData.offsetVectors[0] = mth::float4(-1.0f, -1.0f, -1.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[1] = mth::float4(1.0f, -1.0f, -1.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[2] = mth::float4(-1.0f, 1.0f, -1.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[3] = mth::float4(1.0f, 1.0f, -1.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[4] = mth::float4(-1.0f, -1.0f, 1.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[5] = mth::float4(1.0f, -1.0f, 1.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[6] = mth::float4(-1.0f, 1.0f, 1.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[7] = mth::float4(1.0f, 1.0f, 1.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[8] = mth::float4(0.0f, 0.0f, -1.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[9] = mth::float4(0.0f, 0.0f, 1.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[10] = mth::float4(0.0f, -1.0f, 0.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[11] = mth::float4(0.0f, 1.0f, 0.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[12] = mth::float4(1.0f, 0.0f, 0.0f, 0.0f).Normalized();
			m_occlusionMapPSBufferData.offsetVectors[13] = mth::float4(-1.0f, 0.0f, 0.0f, 0.0f).Normalized();

			for (mth::float4& v : m_occlusionMapPSBufferData.offsetVectors)
				v *= mth::Random(0.25f, 1.0f);

			m_occlusionMapPSBufferData.occlusionRadius = 0.5f;
			m_occlusionMapPSBufferData.occlusionFadeStart = 0.2f;
			m_occlusionMapPSBufferData.occlusionFadeEnd = 2.0f;
			m_occlusionMapPSBufferData.surfaceEpsilon = 0.05f;
		}

		void AmbientOcclusion::CreateBlurTextureResources(ID3D11Device3* device)
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = m_width;
			textureDesc.Height = m_height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;
			ThrowIfFailed(device->CreateTexture2D(&textureDesc, nullptr, &m_blurTex));

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
			ThrowIfFailed(device->CreateShaderResourceView(m_blurTex.Get(), &srvDesc, &m_blurSRV));

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = textureDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			ThrowIfFailed(device->CreateRenderTargetView(m_blurTex.Get(), &rtvDesc, &m_blurRTV));
		}
		void AmbientOcclusion::CreateBlurPixelShader(Graphics& graphics)
		{
			static const char* code = R"(
Texture2D texture_occlusionMap   : register(t0);
Texture2D texture_normalDepthMap : register(t1);
SamplerState wrapSampler         : register(s0);
SamplerState clampSampler        : register(s1);

cbuffer OcclusionData
{
	float2 screenSize;
	float2 pixelDelta;
};

struct PixelInputType
{
	float4 wndpos   : SV_POSITION;
	float3 position : POSITION;
};

float4 main(PixelInputType input) : SV_TARGET
{
	static float weights[] = { 0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f };
	float2 texCoords = input.wndpos.xy / screenSize;
	float4 color = weights[5] * texture_occlusionMap.Sample(clampSampler, texCoords);
	float totalWeight = weights[5];
	float4 centerNormalDepth = texture_normalDepthMap.Sample(clampSampler, texCoords);

	[unroll]
	for (float i = -5.0f; i <= 5.0f; i++)
	{
		if (i == 0.0f) continue;
		float2 tex = texCoords + pixelDelta * i;
		float4 neighborNormalDepth = texture_normalDepthMap.Sample(clampSampler, tex);
		if (dot(neighborNormalDepth.xyz, centerNormalDepth.xyz) >= 0.8f &&
			abs(neighborNormalDepth.w - centerNormalDepth.w) <= 0.2f)
		{
			float weight = weights[i + 5];
			color += weight * texture_occlusionMap.Sample(clampSampler, tex);
			totalWeight += weight;
		}
	}

	return color / totalWeight;
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
			ThrowIfFailed(graphics.Device3D()->CreatePixelShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &m_psBlur));
		}

		void AmbientOcclusion::CreateRandomVectors(ID3D11Device3* device)
		{
			unsigned width = 128;
			unsigned height = 128;

			m_occlusionMapPSBufferData.randVecTexSize.x = static_cast<float>(width);
			m_occlusionMapPSBufferData.randVecTexSize.y = static_cast<float>(height);

			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = width;
			textureDesc.Height = height;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;

			std::vector<mth::float3> vectors(width * height);
			for (auto& v : vectors)
				v = mth::float3(mth::Random(-1.0f, 1.0f), mth::Random(-1.0f, 1.0f), mth::Random(-1.0f, 1.0f)).Normalized();

			D3D11_SUBRESOURCE_DATA data{};
			data.pSysMem = vectors.data();
			data.SysMemPitch = width * sizeof(mth::float3);
			data.SysMemSlicePitch = 0;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
			ThrowIfFailed(device->CreateTexture2D(&textureDesc, &data, &texture));

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
			ThrowIfFailed(device->CreateShaderResourceView(texture.Get(), &srvDesc, &m_randomVectors));
		}
		void AmbientOcclusion::CreateDepthBuffer(ID3D11Device3* device)
		{
			CD3D11_TEXTURE2D_DESC depthBufferDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, m_width, m_height, 1, 1, D3D11_BIND_DEPTH_STENCIL);
			Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBuffer;
			ThrowIfFailed(device->CreateTexture2D(&depthBufferDesc, nullptr, &depthBuffer));

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Flags = 0;
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
			ThrowIfFailed(device->CreateDepthStencilView(depthBuffer.Get(), &dsvDesc, &m_depthStencilView));
		}
		void AmbientOcclusion::CreateRasterizerState(ID3D11Device3* device)
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
			ThrowIfFailed(device->CreateRasterizerState2(&rasterizerDesc, &m_rasterizerState));
		}
		void AmbientOcclusion::CreateSamplers(ID3D11Device3* device)
		{
			D3D11_SAMPLER_DESC samplerDesc{};
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
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
			ThrowIfFailed(device->CreateSamplerState(&samplerDesc, &m_wrapSampler));

			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			ThrowIfFailed(device->CreateSamplerState(&samplerDesc, &m_clampSampler));
		}
		void AmbientOcclusion::CreateScreenQuadModel(Graphics& graphics)
		{
			Vertex vertices[] = {
				{ mth::float3(-1.0f, -1.0f, 1.0f), mth::float2(), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() },
				{ mth::float3(1.0f, -1.0f, 1.0f), mth::float2(), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() },
				{ mth::float3(1.0f, 1.0f,  1.0f), mth::float2(), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() },
				{ mth::float3(-1.0f, 1.0f,  1.0f), mth::float2(), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() }
			};
			unsigned indices[] = {
				0, 3, 2, 2, 1, 0
			};
			m_screenQuad = Model::CreateU(graphics, vertices, ARRAY_SIZE(vertices), indices, ARRAY_SIZE(indices));
		}
		void AmbientOcclusion::CreateScreenQuadVertexShader(Graphics& graphics)
		{
			static const char* code = R"(
cbuffer MatrixBuffer
{
	matrix cameraMatrix;
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
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	output.position = input.position;
	output.wndpos = mul(float4(input.position, 1.0f), cameraMatrix);
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
			ThrowIfFailed(device->CreateVertexShader(byteCode->GetBufferPointer(), byteCode->GetBufferSize(), nullptr, &m_vsScreenQuad));
		}
		void AmbientOcclusion::CreateShaderBuffers(ID3D11Device3* device)
		{
			D3D11_BUFFER_DESC bufferDesc;

			m_screenQuadVSBufferSize = sizeof(mth::float4x4);
			m_occlusionMapPSBufferSize = sizeof(OcclusionPSCB);
			m_blurPSBufferSize = sizeof(mth::float2) * 2;

			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = 0;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.ByteWidth = m_screenQuadVSBufferSize;
			ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, &m_screenQuadVSBuffer));

			bufferDesc.ByteWidth = m_occlusionMapPSBufferSize;
			ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, &m_occlusionMapPSBuffer));

			bufferDesc.ByteWidth = m_blurPSBufferSize;
			ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, &m_blurPSBuffer));
		}

		void WriteConstantBuffer(ID3D11DeviceContext3* context, ID3D11Buffer* buffer, void* data, unsigned dataSize)
		{
			D3D11_MAPPED_SUBRESOURCE resource;
			if (SUCCEEDED(context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
			{
				memcpy(resource.pData, data, dataSize);
				context->Unmap(buffer, 0);
			}
		}
		void AmbientOcclusion::WriteScreenQuadVSBuffer(ID3D11DeviceContext3* context, void* data)
		{
			WriteConstantBuffer(context, m_screenQuadVSBuffer.Get(), data, m_screenQuadVSBufferSize);
		}
		void AmbientOcclusion::WriteOcclusionMapPSBuffer(ID3D11DeviceContext3* context)
		{
			WriteConstantBuffer(context, m_occlusionMapPSBuffer.Get(), &m_occlusionMapPSBufferData, m_occlusionMapPSBufferSize);
		}
		void AmbientOcclusion::WriteBlurPSBuffer(ID3D11DeviceContext3* context, void* data)
		{
			WriteConstantBuffer(context, m_blurPSBuffer.Get(), data, m_blurPSBufferSize);
		}

		void AmbientOcclusion::BlurOcclusionMap(Graphics& graphics)
		{
			auto* context = graphics.Context3D();
			context->PSSetShader(m_psBlur.Get(), nullptr, 0);
			context->PSSetConstantBuffers(0, 1, m_blurPSBuffer.GetAddressOf());
			context->PSSetShaderResources(1, 1, m_normalDepthMapSRV.GetAddressOf());

			mth::float2 screenSize = mth::float2(static_cast<float>(m_width), static_cast<float>(m_height));
			mth::float2 blurBuffer[] = {
				screenSize,
				mth::float2()
			};

			blurBuffer[1] = mth::float2(1.0f / screenSize.x, 0.0f);
			WriteBlurPSBuffer(context, blurBuffer);
			context->OMSetRenderTargets(1, m_blurRTV.GetAddressOf(), nullptr);
			context->PSSetShaderResources(0, 1, m_occlusionSRV.GetAddressOf());
			m_screenQuad->RenderAll(graphics);

			blurBuffer[1] = mth::float2(0.0f, 1.0f / screenSize.y);
			WriteBlurPSBuffer(context, blurBuffer);
			context->OMSetRenderTargets(1, m_occlusionRTV.GetAddressOf(), nullptr);
			context->PSSetShaderResources(0, 1, m_blurSRV.GetAddressOf());
			m_screenQuad->RenderAll(graphics);
		}

		AmbientOcclusion::AmbientOcclusion(Graphics& graphics) :
			m_viewport{ 0.0f, 0.0f, static_cast<float>(graphics.Width()), static_cast<float>(graphics.Height()), 0.0f, 1.0f },
			m_width(max(1, graphics.Width())),
			m_height(max(1, graphics.Height()))
		{
			auto* device = graphics.Device3D();

			CreateNormalDepthMapTextureResources(device);
			CreateNormalDepthMapVertexShader(graphics);
			CreateNormalDepthMapPixelShader(graphics);

			CreateOcclusionMapTextureResources(device);
			CreateOcclusionMapPixelShader(graphics);
			FillOcclusionMapPSBuffer();

			CreateBlurTextureResources(device);
			CreateBlurPixelShader(graphics);

			CreateRandomVectors(device);
			CreateDepthBuffer(device);
			CreateRasterizerState(device);
			CreateSamplers(device);
			CreateScreenQuadModel(graphics);
			CreateScreenQuadVertexShader(graphics);
			CreateShaderBuffers(device);
		}
		AmbientOcclusion::P AmbientOcclusion::CreateP(Graphics& graphics)
		{
			return std::make_shared<AmbientOcclusion>(graphics);
		}
		AmbientOcclusion::U AmbientOcclusion::CreateU(Graphics& graphics)
		{
			return std::make_unique<AmbientOcclusion>(graphics);
		}
		void AmbientOcclusion::Resize(Graphics& graphics)
		{
			auto* device = graphics.Device3D();
			m_viewport.Width = static_cast<float>(graphics.Width());
			m_viewport.Height = static_cast<float>(graphics.Height());
			m_width = graphics.Width();
			m_height = graphics.Height();

			CreateNormalDepthMapTextureResources(device);
			CreateOcclusionMapTextureResources(device);
			CreateBlurTextureResources(device);
			CreateDepthBuffer(device);
		}
		void AmbientOcclusion::SetTextureToRender(Graphics& graphics, unsigned index)
		{
			graphics.Context3D()->PSSetShaderResources(index, 1, m_occlusionSRV.GetAddressOf());
		}
		void AmbientOcclusion::SetAsRenderTarget(Graphics& graphics, Camera& camera)
		{
			float clearColor[] = { 0.0f, 0.0f, 0.0f, 1e5f };
			auto* context = graphics.Context3D();
			ID3D11RenderTargetView* renderTargets[1] = { m_normalDepthMapRTV.Get() };
			graphics.SetVSMatrixBuffer();
			graphics.SetVSBoneBuffer();
			context->RSSetState(m_rasterizerState.Get());
			context->OMSetRenderTargets(1, renderTargets, m_depthStencilView.Get());
			context->RSSetViewports(1, &m_viewport);
			context->ClearRenderTargetView(m_normalDepthMapRTV.Get(), clearColor);
			context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			context->VSSetShader(m_vsNormalDepthMap.Get(), nullptr, 0);
			context->PSSetShader(m_psNormalDepthMap.Get(), nullptr, 0);
		}
		void AmbientOcclusion::RenderOcclusionMap(Graphics& graphics, Camera& camera, unsigned blur)
		{
			auto* context = graphics.Context3D();
			context->RSSetState(m_rasterizerState.Get());
			context->OMSetRenderTargets(1, m_occlusionRTV.GetAddressOf(), nullptr);
			context->RSSetViewports(1, &m_viewport);
			context->VSSetShader(m_vsScreenQuad.Get(), nullptr, 0);
			context->PSSetShader(m_psOcclusionMap.Get(), nullptr, 0);
			context->VSSetConstantBuffers(0, 1, m_screenQuadVSBuffer.GetAddressOf());
			context->PSSetSamplers(0, 1, m_wrapSampler.GetAddressOf());
			context->PSSetSamplers(1, 1, m_clampSampler.GetAddressOf());
			context->PSSetConstantBuffers(0, 1, m_occlusionMapPSBuffer.GetAddressOf());

			mth::float4x4 matrixBuffer[] = {
				camera.ProjectionMatrix()
			};
			WriteScreenQuadVSBuffer(context, matrixBuffer);
			m_occlusionMapPSBufferData.screenSize = mth::float2(static_cast<float>(m_width), static_cast<float>(m_height));
			m_occlusionMapPSBufferData.viewToTexSpace = camera.ProjectionMatrix();
			WriteOcclusionMapPSBuffer(context);

			context->PSSetShaderResources(0, 1, m_normalDepthMapSRV.GetAddressOf());
			context->PSSetShaderResources(1, 1, m_randomVectors.GetAddressOf());
			m_screenQuad->SetBuffersToRender(graphics);
			m_screenQuad->RenderAll(graphics);

			for (unsigned i = 0; i < blur; i++)
				BlurOcclusionMap(graphics);
		}
	}
}