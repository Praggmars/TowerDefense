#pragma once

#include "graphics.h"

namespace Converter
{
	namespace gfx
	{
		class Texture
		{
		public:
			SMART_PTR(Texture)
		protected:
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
			unsigned m_width;
			unsigned m_height;

		protected:
			Texture(unsigned width, unsigned height);

		public:
			Texture(Graphics& graphics, void* data, unsigned width, unsigned height);
			static Texture::P CreateP(Graphics& graphics, void* data, unsigned width, unsigned height);
			static Texture::U CreateU(Graphics& graphics, void* data, unsigned width, unsigned height);

			void SetToRender(Graphics& graphics, unsigned index = 0);
			inline unsigned Width() { return m_width; }
			inline unsigned Height() { return m_height; }
		};
	}
}