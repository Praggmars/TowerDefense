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
				hlp::ThrowIfFailed(D3D11CreateDevice(
					nullptr, D3D_DRIVER_TYPE_WARP, 0, D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevels,
					ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &m_featureLevel, &context));

			hlp::ThrowIfFailed(device.As(&m_device3D));
			hlp::ThrowIfFailed(context.As(&m_context3D));
		}
		void Graphics::CreateRasterizerState()
		{
			D3D11_RASTERIZER_DESC2 rasterizerDesc{};
			rasterizerDesc.AntialiasedLineEnable = true;
			rasterizerDesc.CullMode = D3D11_CULL_BACK;
			rasterizerDesc.DepthBias = 0;
			rasterizerDesc.DepthBiasClamp = 0.0f;
			rasterizerDesc.DepthClipEnable = true;
			rasterizerDesc.FrontCounterClockwise = false;
			rasterizerDesc.MultisampleEnable = true;
			rasterizerDesc.ScissorEnable = false;
			rasterizerDesc.SlopeScaledDepthBias = 0.0f;
			rasterizerDesc.FillMode = D3D11_FILL_SOLID;
			rasterizerDesc.ForcedSampleCount = 0;
			rasterizerDesc.ConservativeRaster = D3D11_CONSERVATIVE_RASTERIZATION_MODE_OFF;
			hlp::ThrowIfFailed(m_device3D->CreateRasterizerState2(&rasterizerDesc, &m_rasterizerState));

			m_context3D->RSSetState(m_rasterizerState.Get());
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
			hlp::ThrowIfFailed(m_device3D->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState));
			m_context3D->OMSetDepthStencilState(m_depthStencilState.Get(), 0);
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
					hlp::ThrowIfFailed(reinterpret_cast<IUnknown*>(m_swapChainPanel)->QueryInterface(IID_PPV_ARGS(&panelNative)));
					hlp::ThrowIfFailed(panelNative->SetSwapChain(m_swapChain.Get()));
				}, Platform::CallbackContext::Any));
		}
		void Graphics::CreateSwapChain()
		{
			if (m_swapChain)
			{
				hlp::ThrowIfFailed(m_swapChain->ResizeBuffers(2, m_width, m_height, DXGI_FORMAT_B8G8R8A8_UNORM, 0));
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
				hlp::ThrowIfFailed(m_device3D.As(&dxgiDevice));

				Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
				hlp::ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

				Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
				hlp::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

				Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
				hlp::ThrowIfFailed(dxgiFactory->CreateSwapChainForComposition(m_device3D.Get(), &swapChainDesc, nullptr, &swapChain));

				hlp::ThrowIfFailed(swapChain.As(&m_swapChain));

				hlp::ThrowIfFailed(dxgiDevice->SetMaximumFrameLatency(1));
			}
		}
		void Graphics::CreateRenderTargetView()
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D1> backBuffer;
			hlp::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
			hlp::ThrowIfFailed(m_device3D->CreateRenderTargetView1(backBuffer.Get(), nullptr, &m_renderTargetView));

			CD3D11_TEXTURE2D_DESC1 depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, m_width, m_height, 1, 1, D3D11_BIND_DEPTH_STENCIL);
			Microsoft::WRL::ComPtr<ID3D11Texture2D1> depthStencil;
			hlp::ThrowIfFailed(m_device3D->CreateTexture2D1(&depthStencilDesc, nullptr, &depthStencil));
			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
			hlp::ThrowIfFailed(m_device3D->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &m_depthStencilView));
		}
		void Graphics::CreateViewport()
		{
			m_viewport = CD3D11_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height));
			m_context3D->RSSetViewports(1, &m_viewport);
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
			ID3D11RenderTargetView* rendeTarget[1] = { m_renderTargetView.Get() };
			m_context3D->OMSetRenderTargets(1, rendeTarget, m_depthStencilView.Get());
			m_context3D->ClearRenderTargetView(m_renderTargetView.Get(), color);
			m_context3D->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
		void Graphics::Present()
		{
			hlp::ThrowIfFailed(m_swapChain->Present(1, 0));
			/*DXGI_PRESENT_PARAMETERS parameters = { 0 };
			HRESULT hr = m_swapChain->Present1(1, 0, &parameters);
			m_context3D->DiscardView1(m_renderTargetView.Get(), nullptr, 0);
			m_context3D->DiscardView1(m_depthStencilView.Get(), nullptr, 0);*/
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