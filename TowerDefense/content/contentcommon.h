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
				gfx::Texture::P texture;
				gfx::Texture::P normalmap;
				phy::Hitbox::P hitbox;
			};
			GameModel turret;
			GameModel enemy;
			GameModel area;

			GameResources(gfx::Graphics& graphics);
		};

		class GameObject :public gfx::Entity
		{
		public:
			SMART_PTR(GameObject)
		private:

		public:
			GameObject(GameResources::GameModel& gameModel);
			static GameObject::P CreateP(GameResources::GameModel& gameModel);
			static GameObject::U CreateU(GameResources::GameModel& gameModel);
		};
	}
}