#pragma once

#include "pch.h"
#include "common/helpers.h"
#include "boundingvolume.h"
#include "graphics/modelloader.h"

namespace TowerDefense
{
	namespace phy
	{
		class Hitbox
		{
		public:
			SMART_PTR(Hitbox)
		private:
			BoundingVolume m_aabb;
			std::vector<mth::float3> m_vertices;
			std::vector<unsigned> m_indices;

		public:
			Hitbox(gfx::ModelLoader& loader);
			Hitbox(mth::float3 vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount);
			Hitbox(gfx::Vertex vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount);
			static Hitbox::P CreateP(gfx::ModelLoader& loader);
			static Hitbox::P CreateP(mth::float3 vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount);
			static Hitbox::P CreateP(gfx::Vertex vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount);
			static Hitbox::U CreateU(gfx::ModelLoader& loader);
			static Hitbox::U CreateU(mth::float3 vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount);
			static Hitbox::U CreateU(gfx::Vertex vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount);

			float DistanceInDirection(mth::float3 origin, mth::float3 direction, mth::float4x4 transform);
		};
	}
}