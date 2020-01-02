#pragma once

#include "graphics/scene.h"
#include "graphics/firstpersoncontroller.h"

namespace TowerDefense
{
	namespace content
	{
		class Game
		{
		public:
			SMART_PTR(Game)

		private:
			gfx::Graphics& m_graphics;
			gfx::Scene::P m_scene;
			gfx::FirstPersonController m_cameraController;
			mth::float2 m_cursor;
			mth::float2 m_windowSize;
			std::vector<gfx::Entity::P> m_baseEntities;
			std::vector<gfx::Entity::P> m_placedEntities;
			gfx::Entity::P m_movingEntity;
			gfx::Entity::P m_selectedEntity;
			Windows::Foundation::IAsyncAction^ m_mainLoopWorker;

		private:
			void CreateScene();
			void Update(float deltaTime);
			void Render();

			void ColorEntitiesByPointing();

		public:
			Game(gfx::Graphics& graphics);
			static Game::P CreateP(gfx::Graphics& graphics);
			static Game::U CreateU(gfx::Graphics& graphics);

			void KeyDown(Windows::System::VirtualKey key);
			void KeyUp(Windows::System::VirtualKey key);
			void MouseDown(mth::float2 cursor);
			void MouseMove(mth::float2 cursor);
			void MouseUp(mth::float2 cursor);
			void WindowSizeChanged(mth::float2 size);

			void PlaceCube();
		};
	}
}