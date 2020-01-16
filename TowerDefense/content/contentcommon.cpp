#include "pch.h"
#include "contentcommon.h"

namespace TowerDefense
{
	namespace content
	{
		GameResources::GameResources(gfx::Graphics& graphics)
		{
			gfx::ModelLoader ml;
			ml.CreateCube(mth::float3(0.05f), mth::float3(0.9f));
			turret.texture = gfx::Texture::CreateColoredTexture(graphics, { 200, 255, 200, 255 }, 16);
			turret.normalmap = gfx::Texture::CreateDefaultNormalmap(graphics, 16);
			turret.model = gfx::Model::CreateP(graphics, ml);
			turret.hitbox = phy::Hitbox::CreateP(ml);

			ml.CreateCube(mth::float3(0.25f), mth::float3(0.5f));
			enemy.texture = gfx::Texture::CreateColoredTexture(graphics, { 255, 200, 200, 255 }, 16);
			enemy.normalmap = gfx::Texture::CreateDefaultNormalmap(graphics, 16);
			enemy.model = gfx::Model::CreateP(graphics, ml);
			enemy.hitbox = phy::Hitbox::CreateP(ml);

			ml.CreateQuad(mth::float2(), mth::float2(11.0f, 9.0f), 0.0f);
			area.texture = gfx::Texture::CreateColoredTexture(graphics, { 200, 200, 255, 255 }, 16);
			area.normalmap = gfx::Texture::CreateDefaultNormalmap(graphics, 16);
			area.model = gfx::Model::CreateP(graphics, ml);
			area.hitbox = phy::Hitbox::CreateP(ml);
		}

		GameObject::GameObject(GameResources::GameModel& gameModel) :
			gfx::Entity(
				gameModel.model,
				gfx::Material::CreateP(gameModel.texture, gameModel.normalmap),
				gameModel.hitbox) {}
		GameObject::P GameObject::CreateP(GameResources::GameModel& gameModel)
		{
			return std::make_shared<GameObject>(gameModel);
		}
		GameObject::U GameObject::CreateU(GameResources::GameModel& gameModel)
		{
			return std::make_unique<GameObject>(gameModel);
		}
	}
}