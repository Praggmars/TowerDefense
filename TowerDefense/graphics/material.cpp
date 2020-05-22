#include "pch.h"
#include "material.h"

namespace TowerDefense
{
	namespace gfx
	{
		Material::Material(Graphics& graphics, ModelLoader& modelLoader, unsigned materialIndex)
		{
			auto& mat = modelLoader.Materials()[materialIndex];
			m_texture = Texture::CreateP(graphics, mat.texture.c_str());
			m_normalmap = Texture::CreateP(graphics, mat.normalmap.c_str());
			m_materialBuffer.diffuseColor = mat.data.diffuseColor;
			m_materialBuffer.textureWeight = mat.data.textureWeight;
			m_materialBuffer.specularColor = mat.data.specularColor;
			m_materialBuffer.specularPower = mat.data.specularPower;
		}
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