#pragma once

#include "texture.h"
#include "modelloader.h"
#include "math/linalg.hpp"

namespace TowerDefense
{
	namespace gfx
	{
		class Material
		{
		private:
			Texture::P m_texture;
			Texture::P m_normalmap;

			CB_MaterialBuffer m_materialBuffer;

		public:
			Material() = default;
			Material(Texture::P texture, Texture::P normalmap, MaterialData& data);

			void SetToRender(Graphics& graphics);

			inline CB_MaterialBuffer& MaterialBuffer() { return m_materialBuffer; }
		};
	}
}