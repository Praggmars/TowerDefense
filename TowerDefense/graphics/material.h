#pragma once

#include "texture.h"
#include "shaderbuffer.h"
#include "math/linalg.hpp"

namespace TowerDefense
{
	namespace gfx
	{
		struct MaterialBuffer
		{
			mth::float4 color;
			float textureWeight;
			float padding[3];
		};

		class Material
		{
		public:
			SMART_PTR(Material)

		private:
			Texture::P m_texture;
			Texture::P m_normalmap;

			MaterialBuffer m_materialBuffer;

		public:
			Material(Texture::P texture, Texture::P normalmap);
			static Material::P CreateP(Texture::P texture, Texture::P normalmap);
			static Material::U CreateU(Texture::P texture, Texture::P normalmap);

			void SetToRender(Graphics& graphics, ShaderBuffer& materialBuffer);

			inline MaterialBuffer& MaterialBuffer() { return m_materialBuffer; }
		};
	}
}