#include "pch.h"
#include "camera.h"

namespace Converter
{
	namespace gfx
	{
		void Camera::SetPosition()
		{
			position = mth::float3x3::Rotation(rotation) * mth::float3(0.0f, 0.0f, -m_distance) - m_center;
			m_viewMatrix = mth::float4x4::RotationCamera(position, rotation);
		}
		void Camera::SetProjection()
		{
			m_projectionMatrix = mth::float4x4::PerspectiveFOV(m_fov, m_screenAspectRatio, m_screenNear, m_screenDepth);
		}
		Camera::Camera() :
			m_screenAspectRatio(1.0f),
			m_fov(mth::PI * 0.25f),
			m_screenNear(0.1f),
			m_screenDepth(1000.0f),
			m_distance(10.0f),
			m_sensitivity(0.005f),
			m_tracking(false)
		{
			SetPosition();
			SetProjection();
		}
		void Camera::StartTracking(mth::float2 cursor)
		{
			m_prevCursor = cursor;
		}
		bool Camera::Track(mth::float2 cursor, bool left, bool right)
		{
			if (!left && !right)
				return false;
			mth::float2 delta = cursor - m_prevCursor;
			m_prevCursor = cursor;
			if (right)
			{
				m_center += mth::float3x3::Rotation(rotation) *
					mth::float3(delta.x * m_sensitivity * m_distance * 0.1f,
						-delta.y * m_sensitivity * m_distance * 0.1f, 0.0f);
			}
			if (left)
			{
				LookUp(-delta.y * m_sensitivity);
				TurnRight(delta.x * m_sensitivity);
			}
			SetPosition();
			return true;
		}
		void Camera::Zoom(float zoom)
		{
			m_distance /= zoom;
			SetPosition();
		}
		void Camera::ScreenAspectRatio(float screenAspectRatio)
		{
			m_screenAspectRatio = screenAspectRatio;
			SetProjection();
		}
		mth::float4x4 Camera::CameraMatrix()
		{
			return m_projectionMatrix * m_viewMatrix;
		}
	}
}