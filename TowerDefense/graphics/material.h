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
			Material(Graphics& graphics, ModelLoader& modelLoader, unsigned materialIndex);
			Material(Texture::P texture, Texture::P normalmap, MaterialData& data);

			inline Texture::P DiffuseTexture() { return m_texture; }
			inline Texture::P Normalmap() { return m_normalmap; }
			inline void ChangeDiffuseTexture(Texture::P texture) { m_texture = texture; }
			inline void ChangeNormalmap(Texture::P normalmap) { m_normalmap = normalmap; }

			void SetToRender(Graphics& graphics);

			inline CB_MaterialBuffer& MaterialBuffer() { return m_materialBuffer; }
		};
	}
}