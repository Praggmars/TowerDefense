#pragma once

#include "common/helpers.h"
#include "math/linalg.hpp"

namespace TowerDefense
{
	namespace gfx
	{
		struct Vertex
		{
			mth::float3 position;
			mth::float2 texcoord;
			mth::float3 normal;
			mth::float3 tangent;
			mth::float3 boneWeights;
			mth::vec4<unsigned> boneIndices;
		};

		struct AABB
		{
			mth::float3 p1, p2;
		};

		struct MaterialData
		{
			mth::float4 diffuseColor;
			mth::float4 specularColor;
			float textureWeight;
			float specularPower;
		};
		struct MaterialLoader
		{
			std::wstring name;
			std::wstring texture;
			std::wstring normalmap;
			MaterialData data;
		};

		struct VertexGroup
		{
			unsigned startIndex;
			unsigned indexCount;
			unsigned materialIndex;
		};

		struct BoneLoader
		{
			std::wstring name;
			mth::float4x4 toBoneSpace;	// transform to bone space
			mth::float3 offset;			// offset from parent
			mth::float3 rotation;		// rotation from parent
			int parentIndex;
			std::vector<int> childIndex;
			int ownIndex;
		};

		class ModelLoader
		{
			std::vector<Vertex> m_vertices;
			std::vector<unsigned> m_indices;
			std::vector<VertexGroup> m_groups;
			std::vector<MaterialLoader> m_materials;
			std::vector<BoneLoader> m_bones;
			AABB m_boundingBox;
			std::wstring m_filename;
			std::wstring m_foldername;

		private:
			void StoreFolderName();

		public:
			ModelLoader();
			ModelLoader(const wchar_t* filename, bool binary);

			void CreateQuad(mth::float2 position, mth::float2 size, float y);
			void CreateCube(mth::float3 position, mth::float3 size);

			void LoadBinary(const wchar_t* filename);
			void LoadText(const wchar_t* filename);
			void Clear();

			void Transform(mth::float4x4 transform);
			void CalculateNormals();
			void CalculateTangents();
			void CalculateBoundingBox();

			inline std::vector<Vertex>& Vertices() { return m_vertices; }
			inline std::vector<unsigned>& Indices() { return m_indices; }
			inline std::vector<VertexGroup>& VertexGroups() { return m_groups; }
			inline std::vector<MaterialLoader>& Materials() { return m_materials; }
			inline const wchar_t* Filename() { return m_filename.c_str(); }
			inline const wchar_t* Foldername() { return m_foldername.c_str(); }
			inline std::vector<BoneLoader>& Bones() { return m_bones; }
			inline AABB BoundingBox() { return m_boundingBox; }
		};
	}
}