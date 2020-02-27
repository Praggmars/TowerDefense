#include "pch.h"
#include "pointlight.h"

namespace TowerDefense
{
	namespace gfx
	{
		PointLight::PointLight() :
			m_projectionMatrix(mth::float4x4::PerspectiveFOV(mth::pi * 0.25f, 1.0f, 0.1f, 1000.0f)) {}
		mth::float4x4 PointLight::LightMatrix()
		{
			return m_projectionMatrix * mth::float4x4::RotationCamera(position, rotation);
		}
	}
}