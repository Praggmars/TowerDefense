#include "pch.h"
#include "placementarea.h"

namespace TowerDefense
{
	namespace content
	{
		PlacementArea::PlacementArea(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox):
			gfx::Entity(model, materials, materialCount, hitbox)
		{

		}
		PlacementArea::P PlacementArea::CreateP(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox)
		{
			return std::make_shared<PlacementArea>(model, materials, materialCount, hitbox);
		}
		PlacementArea::U PlacementArea::CreateU(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox)
		{
			return std::make_unique<PlacementArea>(model, materials, materialCount, hitbox);
		}
		void PlacementArea::PlaceTurret(Turret::P turret)
		{
			m_turret = turret;
		}
		void PlacementArea::DestroyTurret()
		{
			m_turret = nullptr;
		}
		bool PlacementArea::HasTurret()
		{
			return static_cast<bool>(m_turret);
		}
	}
}