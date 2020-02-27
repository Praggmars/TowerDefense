#include "pch.h"
#include "contentcommon.h"

namespace TowerDefense
{
	namespace content
	{
		GameResources::GameResources(gfx::Graphics& graphics)
		{
			auto texture = gfx::Texture::CreateColoredTexture(graphics, { 100, 125, 225, 255 }, 16);
			auto normalmap = gfx::Texture::CreateDefaultNormalmap(graphics, 16);
			gfx::MaterialData materialData = { mth::float4(1.0f), mth::float4(), 1.0f, 1.0f };
			gfx::Material::P material;
			gfx::ModelLoader ml;

			ml.CreateCube(mth::float3(-0.25f), mth::float3(0.5f));
			enemy.model = gfx::Model::CreateP(graphics, ml);
			enemy.materials.push_back(
				gfx::Material::CreateP(
					gfx::Texture::CreateColoredTexture(graphics, { 225, 125, 125, 255 }, 16),
					normalmap,
					materialData));
			enemy.hitbox = phy::Hitbox::CreateP(ml);

			ml.LoadText((graphics.AppDirectory() + L"resources\\map.ttdm").c_str());
			map.model = gfx::Model::CreateP(graphics, ml);
			materialData = ml.Materials()[0].data;
			material = gfx::Material::CreateP(texture, normalmap, materialData);
			map.materials.push_back(
				gfx::Material::CreateP(
					gfx::Texture::CreateColoredTexture(graphics, { 55, 225, 55, 255 }, 16),
					normalmap,
					materialData));
			map.hitbox = phy::Hitbox::CreateP(ml);

			ml.LoadText((graphics.AppDirectory() + L"resources\\base.ttdm").c_str());
			base.model = gfx::Model::CreateP(graphics, ml);
			materialData = ml.Materials()[0].data;
			material = gfx::Material::CreateP(texture, normalmap, materialData);
			base.materials.push_back(
				gfx::Material::CreateP(
					gfx::Texture::CreateColoredTexture(graphics, { 225, 55, 225, 255 }, 16),
					normalmap,
					materialData));
			base.hitbox = phy::Hitbox::CreateP(ml);

			ml.LoadText((graphics.AppDirectory() + L"resources\\heavy.ttdm").c_str());
			turret_heavy.model = gfx::Model::CreateP(graphics, ml);
			materialData = ml.Materials()[0].data;
			material = gfx::Material::CreateP(texture, normalmap, materialData);
			for (size_t i = 0; i < ml.VertexGroups().size(); i++)
				turret_heavy.materials.push_back(material);
			turret_heavy.hitbox = phy::Hitbox::CreateP(ml);

			ml.LoadText((graphics.AppDirectory() + L"resources\\launcher.ttdm").c_str());
			turret_launcher.model = gfx::Model::CreateP(graphics, ml);
			materialData = ml.Materials()[0].data;
			material = gfx::Material::CreateP(texture, normalmap, materialData);
			for (size_t i = 0; i < ml.VertexGroups().size(); i++)
				turret_launcher.materials.push_back(material);
			turret_launcher.hitbox = phy::Hitbox::CreateP(ml);

			ml.LoadText((graphics.AppDirectory() + L"resources\\light.ttdm").c_str());
			turret_light.model = gfx::Model::CreateP(graphics, ml);
			materialData = ml.Materials()[0].data;
			material = gfx::Material::CreateP(texture, normalmap, materialData);
			for (size_t i = 0; i < ml.VertexGroups().size(); i++)
				turret_light.materials.push_back(material);
			turret_light.hitbox = phy::Hitbox::CreateP(ml);
		}

		GameObject::GameObject(GameResources::GameModel& gameModel) :
			gfx::Entity(
				gameModel.model,
				gameModel.materials.data(),
				static_cast<unsigned>(gameModel.materials.size()),
				gameModel.hitbox) {}
		GameObject::GameObject(gfx::Model::P model, gfx::Material::P material, phy::Hitbox::P hitbox) :
			gfx::Entity(model, material, hitbox) {}
		GameObject::P GameObject::CreateP(GameResources::GameModel& gameModel)
		{
			return std::make_shared<GameObject>(gameModel);
		}
		GameObject::U GameObject::CreateU(GameResources::GameModel& gameModel)
		{
			return std::make_unique<GameObject>(gameModel);
		}
		GameObject::P GameObject::CreateP(gfx::Model::P model, gfx::Material::P material, phy::Hitbox::P hitbox)
		{
			return std::make_shared<GameObject>(model, material, hitbox);
		}
		GameObject::U GameObject::CreateU(gfx::Model::P model, gfx::Material::P material, phy::Hitbox::P hitbox)
		{
			return std::make_unique<GameObject>(model, material, hitbox);
		}
	}
}