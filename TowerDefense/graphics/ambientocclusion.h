#pragma once

#include "model.h"
#include "camera.h"

namespace TowerDefense
{
	namespace gfx
	{
		class AmbientOcclusion
		{
			struct OcclusionPSCB
			{
				mth::float4x4 viewToTexSpace;
				mth::float4 offsetVectors[14];
				mth::float2 screenSize;
				mth::float2 randVecTexSize;
				float occlusionRadius;
				float occlusionFadeStart;
				float occlusionFadeEnd;
				float surfaceEpsilon;
			};
		public:
			SMART_PTR(AmbientOcclusion)
		private:
			Microsoft::WRL::ComPtr<ID3D11RasterizerState2> m_rasterizerState;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_normalDepthMapTex;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalDepthMapSRV;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_normalDepthMapRTV;
			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vsNormalDepthMap;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_psNormalDepthMap;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_occlusionTex;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_occlusionSRV;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_occlusionRTV;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_psOcclusionMap;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_occlusionMapPSBuffer;
			unsigned m_occlusionMapPSBufferSize;

			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_blurTex;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_blurSRV;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_blurRTV;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_psBlur;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_blurPSBuffer;
			unsigned m_blurPSBufferSize;

			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_randomVectors;
			Microsoft::WRL::ComPtr<ID3D11SamplerState> m_wrapSampler;
			Microsoft::WRL::ComPtr<ID3D11SamplerState> m_clampSampler;
			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vsScreenQuad;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_screenQuadVSBuffer;
			unsigned m_screenQuadVSBufferSize;
			Model::U m_screenQuad;
			OcclusionPSCB m_occlusionMapPSBufferData;
			D3D11_VIEWPORT m_viewport;
			unsigned m_width;
			unsigned m_height;

		private:
			void CreateNormalDepthMapTextureResources(ID3D11Device3* device);
			void CreateNormalDepthMapVertexShader(Graphics& graphics);
			void CreateNormalDepthMapPixelShader(Graphics& graphics);

			void CreateOcclusionMapTextureResources(ID3D11Device3* device);
			void CreateOcclusionMapPixelShader(Graphics& graphics);
			void FillOcclusionMapPSBuffer();

			void CreateBlurTextureResources(ID3D11Device3* device);
			void CreateBlurPixelShader(Graphics& graphics);

			void CreateRandomVectors(ID3D11Device3* device);
			void CreateDepthBuffer(ID3D11Device3* device);
			void CreateRasterizerState(ID3D11Device3* device);
			void CreateSamplers(ID3D11Device3* device);
			void CreateScreenQuadModel(Graphics& graphics);
			void CreateScreenQuadVertexShader(Graphics& graphics);
			void CreateShaderBuffers(ID3D11Device3* device);

			void WriteScreenQuadVSBuffer(ID3D11DeviceContext3* context, void* data);
			void WriteOcclusionMapPSBuffer(ID3D11DeviceContext3* context);
			void WriteBlurPSBuffer(ID3D11DeviceContext3* context, void* data);

			void BlurOcclusionMap(Graphics& graphics);

		public:
			AmbientOcclusion(Graphics& graphics);
			static AmbientOcclusion::P CreateP(Graphics& graphics);
			static AmbientOcclusion::U CreateU(Graphics& graphics);

			void Resize(Graphics& graphics);

			void SetAsRenderTarget(Graphics& graphics, Camera& camera);
			void SetTextureToRender(Graphics& graphics, unsigned index);
			void RenderOcclusionMap(Graphics& graphics, Camera& camera, unsigned blur = 1);
		};
	}
}