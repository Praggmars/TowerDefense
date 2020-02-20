#include "pch.h"
#include "material.h"

namespace TowerDefense
{
	namespace gfx
	{
		Material::Material(Texture::P texture, Texture::P normalmap) :
			m_texture(texture),
			m_normalmap(normalmap),
			m_materialBuffer{ mth::float4(1.0f), mth::float4(), 0.0f, 1.0f,{ 0.0f, 0.0f } } {}
		Material::P Material::CreateP(Texture::P texture, Texture::P normalmap)
		{
			return std::make_shared<Material>(texture, normalmap);
		}
		Material::U Material::CreateU(Texture::P texture, Texture::P normalmap)
		{
			return std::make_unique<Material>(texture, normalmap);
		}
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