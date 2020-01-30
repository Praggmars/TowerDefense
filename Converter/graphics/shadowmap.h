#pragma once

#include "texture.h"

namespace Converter
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
			Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
			unsigned m_bufferSize;
			D3D11_VIEWPORT m_viewport;
			unsigned m_width;
			unsigned m_height;

		private:
			Microsoft::WRL::ComPtr<ID3D11Texture2D> CreateDepthMap(ID3D11Device3* device);
			void CreateRasterizerState(ID3D11Device3* device);
			void CreateVertexShader(Graphics& graphics);
			void CreatePixelShader(Graphics& graphics);
			void CreateBuffer(ID3D11Device3* device);

		public:
			ShadowMap(Graphics& graphics, unsigned width, unsigned height);
			static ShadowMap::P CreateP(Graphics& graphics, unsigned width, unsigned height);
			static ShadowMap::U CreateU(Graphics& graphics, unsigned width, unsigned height);

			void SetTextureToRender(Graphics& graphics, unsigned index);
			void SetAsRenderTarget(Graphics& graphics);
			void WriteBuffer(Graphics& graphics, void* data);
		};
	}
}