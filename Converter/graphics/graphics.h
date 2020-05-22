#pragma once

#include "common/helpers.h"
#include "math/linalg.hpp"

namespace Converter
{
	namespace gfx
	{
		struct CB_MatrixBuffer
		{
			mth::float4x4 worldMatrix;
			mth::float4x4 viewMatrix;
			mth::float4x4 cameraMatrix;
			mth::float4x4 lightMatrix;
		};
		struct CB_LightBuffer
		{
			mth::float4 diffuseColor;
			mth::float4 ambientColor;
			mth::float3 sourcePosition;
			float shadowMapDelta;
			mth::float3 eyePosition;
			float padding1;
			mth::float2 screenSize;
			float padding2[2];
		};
		struct CB_MaterialBuffer
		{
			mth::float4 diffuseColor;
			mth::float4 specularColor;
			float textureWeight;
			float specularPower;
			float padding[2];
		};

		class Graphics
		{
		public:
			SMART_PTR(Graphics)

		private:
			Microsoft::WRL::ComPtr<ID3D11Device3> m_device3D;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext3> m_context3D;
			Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
			Windows::UI::Xaml::Controls::SwapChainPanel^ m_swapChainPanel;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState2> m_rasterizerState;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView1> m_renderTargetView;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
			Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
			D3D11_VIEWPORT m_viewport;
			D3D_FEATURE_LEVEL m_featureLevel;
			unsigned m_width, m_height;

			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_vsMatrixBuffer;
			unsigned m_vsMatrixBufferSize;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_psLightBuffer;
			unsigned m_psLightBufferSize;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_psMaterialBuffer;
			unsigned m_psMaterialBufferSize;
			Microsoft::WRL::ComPtr<ID3D11SamplerState> m_textureSampler;
			Microsoft::WRL::ComPtr<ID3D11SamplerState> m_shadowSampler;

			Microsoft::WRL::ComPtr<ID3D11Buffer> m_boneBuffer;

			Concurrency::critical_section m_lock;

		private:
			void CreateDevice();
			void CreateRasterizerState();
			void CreateDepthStencilState();
			void CreateBlendState();

			void CreateWindowSizeDependentResources();
			void CreateSwapChain();
			void CreateRenderTargetView();
			void CreateViewport();

			void CreateVertexShader();
			void CreatePixelShader();
			void CreateShaderBuffers();
			void WriteShaderBuffer(ID3D11Buffer* buffer, void* data, unsigned size);
			void CreateSamplerStates();

		public:
			Graphics();
			static Graphics::P CreateP();
			static Graphics::U CreateU();

			void Clear();
			void Clear(float r, float g, float b, float a = 1.0f);
			void Clear(float color[4]);
			void SetScreenAsRenderTarget();
			void Present();

			void WriteVSMatrixBuffer(void* data);
			void WritePSLightBuffer(void* data);
			void WritePSMaterialBuffer(void* data);
			void WriteBoneBuffer(mth::float4x4 bones[], unsigned count);

			void SetSwapChainPanel(Windows::UI::Xaml::Controls::SwapChainPanel^ panel);
			void Resize(unsigned width, unsigned height);

			inline ID3D11Device3* Device3D() { return m_device3D.Get(); }
			inline ID3D11DeviceContext3* Context3D() { return m_context3D.Get(); }
			inline D3D_FEATURE_LEVEL FeatureLevel() { return m_featureLevel; }
			inline void SetVSMatrixBuffer() { m_context3D->VSSetConstantBuffers(0, 1, m_vsMatrixBuffer.GetAddressOf()); }
			inline void SetVSBoneBuffer() { m_context3D->VSSetConstantBuffers(1, 1, m_boneBuffer.GetAddressOf()); }
			inline unsigned Width() { return m_width; }
			inline unsigned Height() { return m_height; }
			inline auto& GetLock() { return m_lock; }
		};
	}
}