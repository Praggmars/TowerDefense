#include "pch.h"
#include "boundingvolume.h"

namespace TowerDefense
{
	namespace phy
	{
		BoundingVolume::BoundingVolume() {}
		BoundingVolume::BoundingVolume(mth::float3 position, mth::float3 size) :
			position(position),
			size(size) {}
		bool BoundingVolume::Intersects(BoundingVolume& bv)
		{
			return phy::Intersects(*this, bv);
		}
		bool Intersects(BoundingVolume& bv1, BoundingVolume& bv2)
		{
			return 
				(bv1.position.x + bv1.size.x > bv2.position.x && bv1.position.x < bv2.position.x + bv2.size.x) &&
				(bv1.position.y + bv1.size.y > bv2.position.y && bv1.position.y < bv2.position.y + bv2.size.y) &&
				(bv1.position.z + bv1.size.z > bv2.position.z && bv1.position.z < bv2.position.z + bv2.size.z);
		}
	}
}