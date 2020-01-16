#pragma once

#include "graphics/texture.h"
#include "graphics/model.h"

namespace TowerDefense
{
	class ResourceLoadingManager
	{
		static ResourceLoadingManager s_Instance;

		std::map<std::wstring, gfx::Texture::W> m_loadedTextures;
		std::map<std::wstring, gfx::Model::W> m_loadedModels;

	public:
		static ResourceLoadingManager& Instance() { return s_Instance; }

		gfx::Texture::P FindTexture(const wchar_t* filename);
		void AddTexture(gfx::Texture::P& texture, const wchar_t* filename);
		gfx::Model::P FindModel(const wchar_t* filename);
		void AddModel(gfx::Model::P& model, const wchar_t* filename);

		void RemoveUnloaded();
	};
}