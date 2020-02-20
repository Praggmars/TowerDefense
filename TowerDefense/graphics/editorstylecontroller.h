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
			mth::float3 m_centerPoint;
			mth::float3 m_rotation;
			mth::float2 m_cursor;

		public:
			EditorStyleController(mth::Positionf& target);

			void SetControllerData(mth::float3 centerPoint, float distance, mth::float3 rotation);

			void MouseMove(mth::float2 cursor, bool rotating, bool moving);
			void Zoom(float zoom);

			void UpdateTarget();
		};
	}
}