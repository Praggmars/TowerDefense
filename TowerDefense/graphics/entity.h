#pragma once

#include "model.h"
#include "material.h"
#include "math/position.hpp"
#include "physics/hitbox.h"
#include "animate.h"

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
			std::vector<Material> m_materials;
			Skeleton m_skeleton;

			phy::Hitbox::P m_hitbox;
			gfx::Animation::P m_animation;

			bool m_visible;

		public:
			Entity(Graphics& graphics, ModelLoader& modelLoader, bool makeHitbox = false);
			Entity(Model::P model, Material* materials, unsigned materialCount, Skeleton& skeleton, phy::Hitbox::P hitbox = nullptr);
			Entity(Model::P model, Material material, Skeleton& skeleton, phy::Hitbox::P hitbox = nullptr);
			static Entity::P CreateP(Graphics& graphics, ModelLoader& modelLoader, bool makeHitbox = false);
			static Entity::P CreateP(Model::P model, Material* materials, unsigned materialCount, Skeleton& skeleton, phy::Hitbox::P hitbox = nullptr);
			static Entity::P CreateP(Model::P model, Material material, Skeleton& skeleton, phy::Hitbox::P hitbox = nullptr);
			static Entity::U CreateU(Graphics& graphics, ModelLoader& modelLoader, bool makeHitbox = false);
			static Entity::U CreateU(Model::P model, Material* materials, unsigned materialCount, Skeleton& skeleton, phy::Hitbox::P hitbox = nullptr);
			static Entity::U CreateU(Model::P model, Material material, Skeleton& skeleton, phy::Hitbox::P hitbox = nullptr);

			void UpdateSkeleton(float deltaTime);
			void RenderBare(Graphics& graphics, CB_MatrixBuffer& matrixBuffer);
			void Render(Graphics& graphics, CB_MatrixBuffer& matrixBuffer);

			template <typename AnimType>
			inline void SetAnimation() { m_animation = AnimType::CreateP(m_skeleton, *this); }

			inline std::vector<Material>& Materials() { return m_materials; }
			inline phy::Hitbox::P Hitbox() { return m_hitbox; }
			inline void Visible(bool visible) { m_visible = visible; }
			inline bool Visible() { return m_visible; }
		};
	}
}