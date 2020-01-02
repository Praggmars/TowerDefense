#pragma once

#include "math/linalg.hpp"

namespace TowerDefense
{
	namespace phy
	{
		class BoundingVolume
		{
		public:
			mth::float3 position;
			mth::float3 size;

		public:
			BoundingVolume();
			BoundingVolume(mth::float3 position, mth::float3 size);

			bool Intersects(BoundingVolume& bv);
		};

		bool Intersects(BoundingVolume& bv1, BoundingVolume& bv2);
	}
}