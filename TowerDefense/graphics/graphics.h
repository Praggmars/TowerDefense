#pragma once

#include "common/helpers.h"

namespace TowerDefense
{
	namespace gfx
	{
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
			D3D11_VIEWPORT m_viewport;
			D3D_FEATURE_LEVEL m_featureLevel;
			unsigned m_width, m_height;
			Concurrency::critical_section m_lock;

		private:
			void CreateDevice();
			void CreateRasterizerState();
			void CreateDepthStencilState();

			void CreateWindowSizeDependentResources();
			void CreateSwapChain();
			void CreateRenderTargetView();
			void CreateViewport();

		public:
			Graphics();
			static Graphics::P CreateP();
			static Graphics::U CreateU();

			void Clear();
			void Clear(float r, float g, float b, float a = 1.0f);
			void Clear(float color[4]);
			void Present();

			void SetSwapChainPanel(Windows::UI::Xaml::Controls::SwapChainPanel^ panel);
			void Resize(unsigned width, unsigned height);

			inline ID3D11Device3* Device3D() { return m_device3D.Get(); }
			inline ID3D11DeviceContext3* Context3D() { return m_context3D.Get(); }
			inline D3D_FEATURE_LEVEL FeatureLevel() { return m_featureLevel; }
			inline unsigned Width() { return m_width; }
			inline unsigned Height() { return m_height; }
			inline auto& GetLock() { return m_lock; }
		};
	}
}