#pragma once

#include "math/position.hpp"

namespace TowerDefense
{
	namespace gfx
	{
		class EditorStyleController
		{
			mth::Positionf& m_target;
			float m_sensitivity;
			float m_distance;
			bool m_ctrlPressed;
			bool m_shiftPressed;
			mth::float3 m_centerPoint;
			mth::float3 m_rotation;
			mth::float2 m_cursor;

		public:
			EditorStyleController(mth::Positionf& target);

			void SetControllerData(mth::float3 centerPoint, float distance, mth::float3 rotation);

			void MouseMove(mth::float2 cursor);
			void KeyDown(Windows::System::VirtualKey key);
			void KeyUp(Windows::System::VirtualKey key);
			void Zoom(float zoom);

			void UpdateTarget();
		};
	}
}