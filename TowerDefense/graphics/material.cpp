#include "pch.h"
#include "material.h"

namespace TowerDefense
{
	namespace gfx
	{
		Material::Material(Texture::P texture, Texture::P normalmap, MaterialData& data) :
			m_texture(texture),
			m_normalmap(normalmap),
			m_materialBuffer{ data.diffuseColor, data.specularColor, data.textureWeight, data.specularPower, { 0.0f, 0.0f } } {}

		void Material::SetToRender(Graphics& graphics)
		{
			graphics.WritePSMaterialBuffer(&m_materialBuffer);
			if (m_texture)
				m_texture->SetToRender(graphics, 0);
			if (m_normalmap)
				m_normalmap->SetToRender(graphics, 1);
		}
	}
}