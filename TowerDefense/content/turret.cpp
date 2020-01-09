#include "pch.h"
#include "turret.h"

namespace TowerDefense
{
	namespace content
	{
		Turret::Turret(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox):
			gfx::Entity(model, materials, materialCount, hitbox)
		{

		}
		Turret::P Turret::CreateP(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox)
		{
			return std::make_shared<Turret>(model, materials, materialCount, hitbox);
		}
		Turret::U Turret::CreateU(gfx::Model::P model, gfx::Material::P materials[], unsigned materialCount, phy::Hitbox::P hitbox)
		{
			return std::make_unique<Turret>(model, materials, materialCount, hitbox);
		}
	}
}