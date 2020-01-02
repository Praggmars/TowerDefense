#pragma once

#include "graphics.h"

namespace TowerDefense
{
	namespace gfx
	{
		class Texture
		{
		public:
			SMART_PTR(Texture)

		private:
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
			unsigned m_width;
			unsigned m_height;

		private:
			void CreateTexture(Graphics& graphics, void* data, unsigned width, unsigned height);
			void LoadFromFile(Graphics& graphics, const wchar_t* filename);

		public:
			Texture(Graphics& graphics, void* data, unsigned width, unsigned height);
			Texture(Graphics& graphics, const wchar_t* filename);
			static Texture::P CreateP(Graphics& graphics, void* data, unsigned width, unsigned height);
			static Texture::P CreateP(Graphics& graphics, const wchar_t* filename);
			static Texture::U CreateU(Graphics& graphics, void* data, unsigned width, unsigned height);
			static Texture::U CreateU(Graphics& graphics, const wchar_t* filename);

			static Texture::P CreateColoredTexture(Graphics& graphics, hlp::Color color, unsigned size);
			static Texture::P CreateDefaultTexture(Graphics& graphics, unsigned size);
			static Texture::P CreateDefaultNormalmap(Graphics& graphics, unsigned size);


			inline unsigned Width() { return m_width; }
			inline unsigned Height() { return m_height; }

			void SetToRender(Graphics& graphics, unsigned index = 0);
		};
	}
}