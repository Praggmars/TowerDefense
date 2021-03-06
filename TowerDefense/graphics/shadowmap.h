#pragma once

#include "graphics.h"

namespace TowerDefense
{
	namespace gfx
	{
		class ShadowMap
		{
		public:
			SMART_PTR(ShadowMap)
		private:
			Microsoft::WRL::ComPtr<ID3D11RasterizerState2> m_rasterizerState;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
			D3D11_VIEWPORT m_viewport;
			unsigned m_size;

		private:
			Microsoft::WRL::ComPtr<ID3D11Texture2D> CreateDepthMap(ID3D11Device3* device);
			void CreateRasterizerState(ID3D11Device3* device);
			void CreateVertexShader(Graphics& graphics);
			void CreatePixelShader(Graphics& graphics);

		public:
			ShadowMap(Graphics& graphics, unsigned size);
			static ShadowMap::P CreateP(Graphics& graphics, unsigned size);
			static ShadowMap::U CreateU(Graphics& graphics, unsigned size);

			void SetTextureToRender(Graphics& graphics, unsigned index);
			void SetAsRenderTarget(Graphics& graphics);

			inline unsigned Size() { return m_size; }
		};
	}
}