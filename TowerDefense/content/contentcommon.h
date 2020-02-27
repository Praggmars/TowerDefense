#pragma once

#include "graphics/entity.h"
#include "physics/hitbox.h"

namespace TowerDefense
{
	namespace content
	{
		struct GameResources
		{
			struct GameModel
			{
				gfx::Model::P model;
				std::vector<gfx::Material::P> materials;
				phy::Hitbox::P hitbox;
			};
			GameModel turret_light;
			GameModel turret_heavy;
			GameModel turret_launcher;
			GameModel map;
			GameModel base;
			GameModel enemy;

			GameResources(gfx::Graphics& graphics);
		};

		class GameObject :public gfx::Entity
		{
		public:
			SMART_PTR(GameObject)
		private:

		public:
			GameObject(GameResources::GameModel& gameModel);
			GameObject(gfx::Model::P model, gfx::Material::P material, phy::Hitbox::P hitbox = nullptr);
			static GameObject::P CreateP(GameResources::GameModel& gameModel);
			static GameObject::U CreateU(GameResources::GameModel& gameModel);
			static GameObject::P CreateP(gfx::Model::P model, gfx::Material::P material, phy::Hitbox::P hitbox = nullptr);
			static GameObject::U CreateU(gfx::Model::P model, gfx::Material::P material, phy::Hitbox::P hitbox = nullptr);
		};
	}
}