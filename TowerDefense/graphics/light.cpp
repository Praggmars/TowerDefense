#include "pch.h"
#include "light.h"

namespace TowerDefense
{
	namespace gfx
	{
		Light::Light(mth::float4x4 projectionMatrix) :
			m_projectionMatrix(projectionMatrix) {}
		mth::float4x4 Light::LightMatrix()
		{
			return m_projectionMatrix * mth::float4x4::RotationCamera(position, rotation);
		}

		PointLight::PointLight(float fov) :
			Light(mth::float4x4::PerspectiveFOV(fov, 1.0f, 0.1f, 1000.0f)) {}

		DirectionalLight::DirectionalLight(float size) :
			Light(mth::float4x4::Orthographic(size, size, 0.1f, 1000.0f)) {}
	}
}