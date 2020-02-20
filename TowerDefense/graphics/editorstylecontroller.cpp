#include "pch.h"
#include "editorstylecontroller.h"

namespace TowerDefense
{
	namespace gfx
	{
		EditorStyleController::EditorStyleController(mth::Positionf& target) :
			m_target(target),
			m_sensitivity(0.005f),
			m_distance(10.0f) {}
		void EditorStyleController::SetControllerData(mth::float3 centerPoint, float distance, mth::float3 rotation)
		{
			m_centerPoint = centerPoint;
			m_distance = distance;
			m_rotation = rotation;
		}
		void EditorStyleController::MouseMove(mth::float2 cursor, bool rotating, bool moving)
		{
			mth::float2 delta = (cursor - m_cursor) * m_sensitivity;
			m_cursor = cursor;
			if (rotating)
			{
				m_rotation.x += delta.y;
				m_rotation.y += delta.x;
				UpdateTarget();
			}
			if (moving)
			{
				m_centerPoint += mth::float3x3::Rotation(m_rotation) *
					mth::float3(delta.x * m_distance * 0.1f,
						-delta.y * m_distance * 0.1f, 0.0f);
				UpdateTarget();
			}
		}
		void EditorStyleController::Zoom(float zoom)
		{
			m_distance /= zoom;
			UpdateTarget();
		}
		void EditorStyleController::UpdateTarget()
		{
			m_target.position = mth::float3x3::Rotation(m_rotation) * mth::float3(0.0f, 0.0f, -m_distance) - m_centerPoint;
			m_target.rotation = m_rotation;
		}
	}
}