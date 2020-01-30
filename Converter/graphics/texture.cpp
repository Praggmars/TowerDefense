#include "pch.h"
#include "texture.h"

namespace Converter
{
	namespace gfx
	{
		Texture::Texture(unsigned width, unsigned height) :
			m_width(width),
			m_height(height) {}
		Texture::Texture(Graphics& graphics, void* data, unsigned width, unsigned height) :
			m_width(width),
			m_height(height)
		{
			ID3D11Device* device = graphics.Device3D();
			ID3D11DeviceContext* context = graphics.Context3D();
			HRESULT hr;
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

			D3D11_TEXTURE2D_DESC t2dd{};
			t2dd.Width = width;
			t2dd.Height = height;
			t2dd.MipLevels = 0;
			t2dd.ArraySize = 1;
			t2dd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			t2dd.SampleDesc.Count = 1;
			t2dd.SampleDesc.Quality = 0;
			t2dd.CPUAccessFlags = 0;
			t2dd.Usage = D3D11_USAGE_DEFAULT;
			t2dd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			t2dd.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

			hr = device->CreateTexture2D(&t2dd, nullptr, &texture);
			if (FAILED(hr))
				throw std::exception("Failed to create texture");
			context->UpdateSubresource(texture.Get(), 0, nullptr, data, width * 4, 0);

			D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
			srvd.Format = t2dd.Format;
			srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvd.Texture2D.MostDetailedMip = 0;
			srvd.Texture2D.MipLevels = -1;
			hr = device->CreateShaderResourceView(texture.Get(), &srvd, &m_shaderResourceView);
			if (FAILED(hr))
				throw std::exception("Failed to create shader resource view");
			context->GenerateMips(m_shaderResourceView.Get());
		}
		Texture::P Texture::CreateP(Graphics& graphics, void* data, unsigned width, unsigned height)
		{
			return std::make_shared<Texture>(graphics, data, width, height);
		}
		Texture::U Texture::CreateU(Graphics& graphics, void* data, unsigned width, unsigned height)
		{
			return std::make_unique<Texture>(graphics, data, width, height);
		}
		void Texture::SetToRender(Graphics& graphics, unsigned index)
		{
			graphics.Context3D()->PSSetShaderResources(index, 1, m_shaderResourceView.GetAddressOf());
		}
	}
}