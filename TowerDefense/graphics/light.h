#pragma once

#include "math/position.hpp"

namespace TowerDefense
{
	namespace gfx
	{
		class Light : public mth::Positionf
		{
		protected:
			mth::float4x4 m_projectionMatrix;

		protected:
			Light(mth::float4x4 projectionMatrix);

		public:
			mth::float4x4 LightMatrix();
		};

		class PointLight : public Light
		{
		public:
			PointLight(float fov);
		};

		class DirectionalLight : public Light
		{
		public:
			DirectionalLight(float size);
		};
	}
}