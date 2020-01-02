#pragma once

#include "model.h"
#include "material.h"
#include "math/position.hpp"
#include "physics/hitbox.h"

namespace TowerDefense
{
	namespace gfx
	{
		class Entity : public mth::Positionf
		{
		public:
			SMART_PTR(Entity)

		private:
			Model::P m_model;
			std::vector<Material::P> m_materials;

			phy::Hitbox::P m_hitbox;

			bool m_visible;

		public:
			Entity(Graphics& graphics, ModelLoader& modelLoader, bool makeHitbox = false);
			Entity(Graphics& graphics, Model::P model, Material::P* materials, unsigned materialCount, phy::Hitbox::P hitbox = nullptr);
			static Entity::P CreateP(Graphics& graphics, ModelLoader& modelLoader, bool makeHitbox = false);
			static Entity::P CreateP(Graphics& graphics, Model::P model, Material::P* materials, unsigned materialCount, phy::Hitbox::P hitbox = nullptr);
			static Entity::U CreateU(Graphics& graphics, ModelLoader& modelLoader, bool makeHitbox = false);
			static Entity::U CreateU(Graphics& graphics, Model::P model, Material::P* materials, unsigned materialCount, phy::Hitbox::P hitbox = nullptr);

			void Render(Graphics& graphics, ShaderBuffer& materialBuffer);

			inline Model::P Model() { return m_model; }
			inline std::vector<Material::P>& Materials() { return m_materials; }
			inline phy::Hitbox::P Hitbox() { return m_hitbox; }
			inline void Visible(bool visible) { m_visible = visible; }
			inline bool Visible() { return m_visible; }
		};
	}
}