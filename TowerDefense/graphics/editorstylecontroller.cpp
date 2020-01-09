#include "pch.h"
#include "editorstylecontroller.h"

namespace TowerDefense
{
	namespace gfx
	{
		EditorStyleController::EditorStyleController(mth::Positionf& target) :
			m_target(target),
			m_sensitivity(0.005f),
			m_distance(10.0f),
			m_ctrlPressed(false),
			m_shiftPressed(false) {}
		void EditorStyleController::SetControllerData(mth::float3 centerPoint, float distance, mth::float3 rotation)
		{
			m_centerPoint = centerPoint;
			m_distance = distance;
			m_rotation = rotation;
		}
		void EditorStyleController::MouseMove(mth::float2 cursor)
		{
			mth::float2 delta = (cursor - m_cursor) * m_sensitivity;
			m_cursor = cursor;
			if (m_ctrlPressed)
			{
				m_rotation.x += delta.y;
				m_rotation.y += delta.x;
				UpdateTarget();
			}
			else if (m_shiftPressed)
			{
				m_centerPoint += mth::float3x3::Rotation(m_rotation) *
					mth::float3(delta.x * m_distance * 0.1f,
						-delta.y * m_distance * 0.1f, 0.0f);
				UpdateTarget();
			}
		}
		void EditorStyleController::KeyDown(Windows::System::VirtualKey key)
		{
			switch (key)
			{
			case Windows::System::VirtualKey::Control:
				m_ctrlPressed = true;
				break;
			case Windows::System::VirtualKey::Shift:
				m_shiftPressed = true;
				break;
			}
		}
		void EditorStyleController::KeyUp(Windows::System::VirtualKey key)
		{
			switch (key)
			{
			case Windows::System::VirtualKey::Control:
				m_ctrlPressed = false;
				break;
			case Windows::System::VirtualKey::Shift:
				m_shiftPressed = false;
				break;
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