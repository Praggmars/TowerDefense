#pragma once

#include "pch.h"
#include "modelloader.h"

namespace TowerDefense
{
	namespace gfx
	{
		struct Bone
		{
			mth::float4x4 toBoneSpace;	// transform to bone space
			mth::float4x4 offset;		// offset from parent
			mth::float4x4 transform;	// translation for animation
			int parentIndex;
			std::vector<int> childIndex;
			int ownIndex;
		};

		class Skeleton
		{
		private:
			std::vector<Bone> m_skeleton;
			std::vector<mth::float4x4> m_matrixBuffer;
			unsigned m_boneCount;

		public:
			Skeleton();
			Skeleton(ModelLoader& modelLoader);

			void Update();

			inline std::vector<Bone>& Bones() { return m_skeleton; }
			inline unsigned BoneCount() { return static_cast<unsigned>(m_skeleton.size()); }
			inline mth::float4x4* MatrixBuffer() { return m_matrixBuffer.data(); }
		};
	}
}