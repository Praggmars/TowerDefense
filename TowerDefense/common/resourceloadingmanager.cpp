#include "pch.h"
#include "resourceloadingmanager.h"

namespace TowerDefense
{
	namespace hlp
	{
		ResourceLoadingManager ResourceLoadingManager::s_Instance;

		gfx::Texture::P ResourceLoadingManager::FindTexture(const wchar_t* filename)
		{
			auto iter = m_loadedTextures.find(filename);
			if (iter == m_loadedTextures.end())
				return nullptr;
			auto tex = iter->second;
			if (!tex.expired())
				return tex.lock();
			m_loadedTextures.erase(iter);
			return nullptr;
		}
		void ResourceLoadingManager::AddTexture(gfx::Texture::P& texture, const wchar_t* filename)
		{
			m_loadedTextures[filename] = texture;
		}
		gfx::Model::P ResourceLoadingManager::FindModel(const wchar_t* filename)
		{
			auto iter = m_loadedModels.find(filename);
			if (iter == m_loadedModels.end())
				return nullptr;
			auto model = iter->second;
			if (!model.expired())
				return model.lock();
			m_loadedModels.erase(iter);
			return nullptr;
		}
		void ResourceLoadingManager::AddModel(gfx::Model::P& model, const wchar_t* filename)
		{
			m_loadedModels[filename] = model;
		}
		void ResourceLoadingManager::RemoveUnloaded()
		{
			auto iterm = m_loadedModels.begin();
			while (iterm != m_loadedModels.end())
			{
				auto current = iterm;
				iterm++;
				if (current->second.expired())
					m_loadedModels.erase(current);
			}

			auto itert = m_loadedTextures.begin();
			while (itert != m_loadedTextures.end())
			{
				auto current = itert;
				itert++;
				if (current->second.expired())
					m_loadedTextures.erase(current);
			}
		}
	}
}