#pragma once

#include "turret.h"

namespace TowerDefense
{
	namespace content
	{
		class PlacementArea :public gfx::Entity
		{
		public:
			SMART_PTR(PlacementArea)
		private:
			Turret::P m_turret;

		public:
			PlacementArea(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox);
			static PlacementArea::P CreateP(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox);
			static PlacementArea::U CreateU(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox);

			void PlaceTurret(Turret::P turret);
			void DestroyTurret();
			bool HasTurret();
		};
	}
}