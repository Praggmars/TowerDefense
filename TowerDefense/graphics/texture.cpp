#include "pch.h"
#include "common/image.h"
#include "common/resourceloadingmanager.h"

namespace TowerDefense
{
	namespace gfx
	{
		void Texture::CreateTexture(Graphics& graphics, void* data, unsigned width, unsigned height)
		{
			ID3D11Device* device = graphics.Device3D();
			ID3D11DeviceContext* context = graphics.Context3D();
			Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

			m_width = width;
			m_height = height;

			D3D11_TEXTURE2D_DESC t2dd{};
			t2dd.Width = m_width;
			t2dd.Height = m_height;
			t2dd.MipLevels = 0;
			t2dd.ArraySize = 1;
			t2dd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			t2dd.SampleDesc.Count = 1;
			t2dd.SampleDesc.Quality = 0;
			t2dd.CPUAccessFlags = 0;
			t2dd.Usage = D3D11_USAGE_DEFAULT;
			t2dd.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			t2dd.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			ThrowIfFailed(device->CreateTexture2D(&t2dd, nullptr, &texture));
			context->UpdateSubresource(texture.Get(), 0, nullptr, data, m_width * 4, 0);

			D3D11_SHADER_RESOURCE_VIEW_DESC srvd{};
			srvd.Format = t2dd.Format;
			srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvd.Texture2D.MostDetailedMip = 0;
			srvd.Texture2D.MipLevels = -1;
			ThrowIfFailed(device->CreateShaderResourceView(texture.Get(), &srvd, &m_shaderResourceView));
			context->GenerateMips(m_shaderResourceView.Get());
		}
		void Texture::LoadFromFile(Graphics& graphics, const wchar_t* filename)
		{
			Image img(filename);
			CreateTexture(graphics, img.Pixels(), img.Width(), img.Height());
		}
		Texture::Texture(Graphics& graphics, void* data, unsigned width, unsigned height)
		{
			CreateTexture(graphics, data, width, height);
		}
		Texture::Texture(Graphics& graphics, const wchar_t* filename)
		{
			LoadFromFile(graphics, filename);
		}
		Texture::P Texture::CreateP(Graphics& graphics, void* data, unsigned width, unsigned height)
		{
			return std::make_shared<Texture>(graphics, data, width, height);
		}
		Texture::P Texture::CreateP(Graphics& graphics, const wchar_t* filename)
		{
			auto texture = ResourceLoadingManager::Instance().FindTexture(filename);
			if (!texture)
			{
				texture = std::make_shared<Texture>(graphics, filename);
				ResourceLoadingManager::Instance().AddTexture(texture, filename);
			}
			return texture;
		}
		Texture::U Texture::CreateU(Graphics& graphics, void* data, unsigned width, unsigned height)
		{
			return std::make_unique<Texture>(graphics, data, width, height);
		}
		Texture::U Texture::CreateU(Graphics& graphics, const wchar_t* filename)
		{
			return std::make_unique<Texture>(graphics, filename);
		}
		Texture::P Texture::CreateColoredTexture(Graphics& graphics, Color color, unsigned size)
		{
			std::vector<Color> pixels(size * size);
			for (Color& c : pixels)
				c = color;
			return std::make_shared<Texture>(graphics, pixels.data(), size, size);
		}
		Texture::P Texture::CreateDefaultTexture(Graphics& graphics, unsigned size)
		{
			return CreateColoredTexture(graphics, { 0xff, 0xff, 0xff, 0xff }, size);
		}
		Texture::P Texture::CreateDefaultNormalmap(Graphics& graphics, unsigned size)
		{
			return CreateColoredTexture(graphics, { 0x7f, 0x7f, 0xff, 0xff }, size);
		}
		void Texture::SetToRender(Graphics& graphics, unsigned index)
		{
			graphics.Context3D()->PSSetShaderResources(index, 1, m_shaderResourceView.GetAddressOf());
		}
	}
}