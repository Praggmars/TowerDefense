#pragma once

#include "math/position.hpp"

namespace TowerDefense
{
	namespace gfx
	{
		class FirstPersonController
		{
			mth::Positionf& m_target;
			float m_speed;
			float m_fastSpeed;
			float m_sensitivity;
			mth::float2 m_cursor;
			char m_inputFlags;

			enum {
				FLAG_W = 1,
				FLAG_A = 2,
				FLAG_S = 4,
				FLAG_D = 8,
				FLAG_SPACE = 16,
				FLAG_CTRL = 32,
				FLAG_SHIFT = 64,
				FLAG_MOUSE = 128
			};

		public:
			FirstPersonController(mth::Positionf& target);

			void MouseDown(mth::float2 cursor);
			void MouseMove(mth::float2 cursor);
			void MouseUp();

			void KeyDown(Windows::System::VirtualKey key);
			void KeyUp(Windows::System::VirtualKey key);
			void Update(float deltaTime);
		};
	}
}