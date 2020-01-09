#pragma once

#include "graphics/entity.h"

namespace TowerDefense
{
	namespace content
	{
		class Turret :public gfx::Entity
		{
		public:
			SMART_PTR(Turret)
		private:

		public:
			Turret(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox);
			static Turret::P CreateP(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox);
			static Turret::U CreateU(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox);
		};
	}
}