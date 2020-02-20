#include "pch.h"
#include "entity.h"

namespace TowerDefense
{
	namespace gfx
	{
		Entity::Entity(Graphics& graphics, ModelLoader& modelLoader, bool makeHitbox) :
			m_visible(true)
		{
			m_model = Model::CreateP(graphics, modelLoader);
			m_materials.resize(modelLoader.Materials().size());
			for (unsigned i = 0; i < modelLoader.Materials().size(); i++)
				m_materials[i] = Material::CreateP(
					Texture::CreateP(graphics, modelLoader.Materials()[i].texture.c_str()),
					Texture::CreateP(graphics, modelLoader.Materials()[i].normalmap.c_str()));
			if (makeHitbox)
				m_hitbox = phy::Hitbox::CreateP(modelLoader);
		}
		Entity::Entity(Model::P model, Material::P* materials, unsigned materialCount, phy::Hitbox::P hitbox) :
			m_visible(true)
		{
			m_model = model;
			m_materials.resize(materialCount);
			for (unsigned i = 0; i < materialCount; i++)
				m_materials[i] = materials[i];
			m_hitbox = hitbox;
		}
		Entity::Entity(Model::P model, Material::P material, phy::Hitbox::P hitbox) :
			Entity(model, &material, 1, hitbox) {}
		Entity::P Entity::CreateP(Graphics& graphics, ModelLoader& modelLoader, bool makeHitbox)
		{
			return std::make_shared<Entity>(graphics, modelLoader, makeHitbox);
		}
		Entity::P Entity::CreateP(Model::P model, Material::P* materials, unsigned materialCount, phy::Hitbox::P hitbox)
		{
			return std::make_shared<Entity>(model, materials, materialCount, hitbox);
		}
		Entity::P Entity::CreateP(Model::P model, Material::P material, phy::Hitbox::P hitbox)
		{
			return std::make_shared<Entity>(model, material, hitbox);
		}
		Entity::U Entity::CreateU(Graphics& graphics, ModelLoader& modelLoader, bool makeHitbox)
		{
			return std::make_unique<Entity>(graphics, modelLoader, makeHitbox);
		}
		Entity::U Entity::CreateU(Model::P model, Material::P* materials, unsigned materialCount, phy::Hitbox::P hitbox)
		{
			return std::make_unique<Entity>(model, materials, materialCount, hitbox);
		}
		Entity::U Entity::CreateU(Model::P model, Material::P material, phy::Hitbox::P hitbox)
		{
			return std::make_unique<Entity>(model, material, hitbox);
		}
		void Entity::Render(Graphics& graphics, CB_MatrixBuffer& matrixBuffer)
		{
			if (m_visible)
			{
				matrixBuffer.worldMatrix = WorldMatrix();
				graphics.WriteVSMatrixBuffer(&matrixBuffer);
				m_model->SetBuffersToRender(graphics);
				for (unsigned i = 0; i < m_materials.size(); i++)
				{
					m_materials[i]->SetToRender(graphics);
					m_model->RenderGroup(graphics, i);
				}
			}
		}
	}
}