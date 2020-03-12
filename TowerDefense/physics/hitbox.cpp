#include "pch.h"
#include "hitbox.h"
#include "math/geometry3d.hpp"

namespace TowerDefense
{
	namespace phy
	{
		Hitbox::Hitbox(gfx::ModelLoader& loader) :
			Hitbox(loader.Vertices().data(), static_cast<unsigned>(loader.Vertices().size()), loader.Indices().data(), static_cast<unsigned>(loader.Indices().size())) {}
		Hitbox::Hitbox(mth::float3 vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount) :
			m_vertices(vertexCount),
			m_indices(indexCount)
		{
			mth::float3 minCoord(vertices[0]), maxCoord(minCoord);
			for (size_t i = 0; i < m_vertices.size(); i++)
			{
				mth::float3 p = vertices[i];
				m_vertices[i] = p;
				if (p.x < minCoord.x) minCoord.x = p.x;
				if (p.y < minCoord.y) minCoord.y = p.y;
				if (p.z < minCoord.z) minCoord.z = p.z;
				if (p.x > maxCoord.x) maxCoord.x = p.x;
				if (p.y > maxCoord.y) maxCoord.y = p.y;
				if (p.z > maxCoord.z) maxCoord.z = p.z;
			}
			memcpy(m_indices.data(), indices, m_indices.size() * sizeof(unsigned));
			m_aabb.position = minCoord;
			m_aabb.size = maxCoord - minCoord;
		}
		Hitbox::Hitbox(gfx::Vertex vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount) :
			m_vertices(vertexCount),
			m_indices(indexCount)
		{
			mth::float3 minCoord(vertices[0].position), maxCoord(minCoord);
			for (size_t i = 0; i < m_vertices.size(); i++)
			{
				mth::float3 p = vertices[i].position;
				m_vertices[i] = p;
				if (p.x < minCoord.x) minCoord.x = p.x;
				if (p.y < minCoord.y) minCoord.y = p.y;
				if (p.z < minCoord.z) minCoord.z = p.z;
				if (p.x > maxCoord.x) maxCoord.x = p.x;
				if (p.y > maxCoord.y) maxCoord.y = p.y;
				if (p.z > maxCoord.z) maxCoord.z = p.z;
			}
			memcpy(m_indices.data(), indices, m_indices.size() * sizeof(unsigned));
			m_aabb.position = minCoord;
			m_aabb.size = maxCoord - minCoord;
		}
		Hitbox::P Hitbox::CreateP(gfx::ModelLoader& loader)
		{
			return std::make_shared<Hitbox>(loader);
		}
		Hitbox::P Hitbox::CreateP(mth::float3 vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount)
		{
			return std::make_shared<Hitbox>(vertices, vertexCount, indices, indexCount);
		}
		Hitbox::P Hitbox::CreateP(gfx::Vertex vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount)
		{
			return std::make_shared<Hitbox>(vertices, vertexCount, indices, indexCount);
		}
		Hitbox::U Hitbox::CreateU(gfx::ModelLoader& loader)
		{
			return std::make_unique<Hitbox>(loader);
		}
		Hitbox::U Hitbox::CreateU(mth::float3 vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount)
		{
			return std::make_unique<Hitbox>(vertices, vertexCount, indices, indexCount);
		}
		Hitbox::U Hitbox::CreateU(gfx::Vertex vertices[], unsigned vertexCount, unsigned indices[], unsigned indexCount)
		{
			return std::make_unique<Hitbox>(vertices, vertexCount, indices, indexCount);
		}
		float Hitbox::DistanceInDirection(mth::float3 origin, mth::float3 direction, mth::float4x4 transform)
		{
			float distance = INFINITY;
			for (unsigned i = 0; i < m_indices.size(); i += 3)
			{
				mth::Triangle3Df tri(
					mth::Transform(transform, m_vertices[m_indices[i + 0]]),
					mth::Transform(transform, m_vertices[m_indices[i + 1]]),
					mth::Transform(transform, m_vertices[m_indices[i + 2]]));
				float tmpDistance = tri.Plain().DirectionalDistance(origin, direction);
				if (distance > tmpDistance&&
					tri.IsPointOver(origin + tmpDistance * direction))
					distance = tmpDistance;
			}
			return distance;
		}
	}
}