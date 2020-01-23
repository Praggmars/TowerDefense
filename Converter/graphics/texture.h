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
		private:
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

		public:
			Texture(Graphics& graphics, void* data, unsigned width, unsigned height);
			static Texture::P CreateP(Graphics& graphics, void* data, unsigned width, unsigned height);
			static Texture::U CreateU(Graphics& graphics, void* data, unsigned width, unsigned height);

			void SetToRender(Graphics& graphics, unsigned index = 0);
		};
	}
}