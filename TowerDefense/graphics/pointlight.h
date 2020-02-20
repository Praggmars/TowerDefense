#pragma once

#include "math/position.hpp"

namespace TowerDefense
{
	namespace gfx
	{
		class PointLight : public mth::Positionf
		{
			mth::float4x4 m_projectionMatrix;

		public:
			PointLight();

			mth::float4x4 LightMatrix();
		};
	}
}