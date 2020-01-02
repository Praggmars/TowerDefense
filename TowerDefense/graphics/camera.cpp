#include "pch.h"
#include "camera.h"

namespace TowerDefense
{
	namespace gfx
	{
		void Camera::SetProjectionMatrix()
		{
			m_projectionMatrix = mth::float4x4::PerspectiveFOV(m_fieldOfView, m_screenAspect, m_screenNear, m_screenDepth);
		}
		Camera::Camera(float screenAspect, float fov, float screenNear, float screenDepth) :
			m_screenAspect(screenAspect),
			m_fieldOfView(fov),
			m_screenNear(screenNear),
			m_screenDepth(screenDepth)
		{
			SetProjectionMatrix();
		}
		void Camera::SetScreenAspect(float screenAspect)
		{
			m_screenAspect = screenAspect;
			SetProjectionMatrix();
		}
		void Camera::SetParams(float screenAspect, float fov, float screenNear, float screenDepth)
		{
			m_screenAspect = screenAspect;
			m_fieldOfView = fov;
			m_screenNear = screenNear;
			m_screenDepth = screenDepth;
			SetProjectionMatrix();
		}
		mth::float3 Camera::DirectionFromPoint(mth::float2 point, mth::float2 windowSize)
		{
			mth::float4 dir(
				(point.x - windowSize.x * 0.5f) / windowSize.x * 2.0f,
				(windowSize.y - point.y - windowSize.y * 0.5f) / windowSize.y * 2.0f,
				1.0f, 1.0f);

			return (mth::float3)((m_projectionMatrix * mth::float4x4::RotationCamera(mth::float3(), rotation)).Inverse() * dir);
		}
		void Camera::Update()
		{
			m_viewMatrix = mth::float4x4::RotationCamera(position, rotation);
		}
		mth::float4x4 Camera::ViewMatrix()
		{
			return m_viewMatrix;
		}
		mth::float4x4 Camera::ProjectionMatrix()
		{
			return m_projectionMatrix;
		}
		mth::float4x4 Camera::CameraMatrix()
		{
			return m_projectionMatrix * m_viewMatrix;
		}
	}
}