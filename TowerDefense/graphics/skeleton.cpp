#include "skeleton.h"

namespace TowerDefense
{
	namespace gfx
	{
		Skeleton::Skeleton()
		{
			m_boneCount = 1;
			m_matrixBuffer.resize(m_boneCount, mth::float4x4::Identity());
			Bone bone;
			bone.toBoneSpace = mth::float4x4::Identity();
			bone.offset = mth::float4x4::Identity();
			bone.transform = mth::float4x4::Identity();
			bone.parentIndex = -1;
			bone.ownIndex = 0;
			m_skeleton.push_back(bone);
		}
		Skeleton::Skeleton(ModelLoader& modelLoader)
		{
			m_boneCount = static_cast<unsigned>(modelLoader.Bones().size());
			m_skeleton.reserve(m_boneCount);
			m_matrixBuffer.resize(m_boneCount, mth::float4x4::Identity());
			for (BoneLoader& b : modelLoader.Bones())
			{
				Bone bone;
				bone.toBoneSpace = b.toBoneSpace;
				bone.offset = mth::float4x4::RotationTranslation(b.rotation, b.offset);
				bone.transform = mth::float4x4::Identity();
				bone.parentIndex = b.parentIndex;
				bone.childIndex = b.childIndex;
				bone.ownIndex = b.ownIndex;
				m_skeleton.push_back(bone);
			}
		}
		void Skeleton::Update()
		{
			for (unsigned i = 0; i < m_boneCount; i++)
			{
				int parentIndex = m_skeleton[i].parentIndex;
				m_matrixBuffer[i] = (parentIndex < 0 ? m_skeleton[i].offset : m_matrixBuffer[parentIndex] * m_skeleton[i].offset) * m_skeleton[i].transform;
			}
			for (unsigned i = 0; i < m_boneCount; i++)
				m_matrixBuffer[i] = m_matrixBuffer[i] * m_skeleton[i].toBoneSpace;
		}
	}
}