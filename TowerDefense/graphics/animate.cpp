#include "animate.h"

namespace TowerDefense::gfx
{
	Animation::Animation(gfx::Skeleton& skeleton, mth::Positionf& target) :
		m_skeleton(skeleton),
		m_target(target) {}

	void DuckAnimation::MoveGlobal()
	{
		auto& bones = m_skeleton.Bones();
		bones[0].transform = mth::float4x4::Translation(0.0f, 0.0f, 0.0f);
	}
	void DuckAnimation::MoveTail(float rotval)
	{
		auto& bones = m_skeleton.Bones();
		bones[14].transform = mth::float4x4::Rotation(rotval * 3.0f, rotval * 2.0f, 0.0f);
		bones[15].transform = mth::float4x4::RotationX(rotval * 10.0f);
	}
	void DuckAnimation::MoveHead(float rotval)
	{
		auto& bones = m_skeleton.Bones();
		bones[1].transform = mth::float4x4::RotationX(rotval);
		bones[2].transform = mth::float4x4::RotationX(rotval * 3.0f);
	}
	void DuckAnimation::MoveLeftLeg(float rotval)
	{
		auto& bones = m_skeleton.Bones();
		bones[4].transform = mth::float4x4::Rotation(rotval, rotval * 2.0f, 0.0f);
		bones[5].transform = mth::float4x4::RotationX(rotval * 10.0f);
		bones[6].transform = mth::float4x4::RotationX(rotval * -10.0f);
		bones[7].transform = mth::float4x4::RotationX(rotval * 3.0f);
	}
	void DuckAnimation::MoveRightLeg(float rotval)
	{
		auto& bones = m_skeleton.Bones();
		bones[9].transform = mth::float4x4::Rotation(-rotval, rotval * -2.0f, 0.0f);
		bones[10].transform = mth::float4x4::RotationX(rotval * -10.0f);
		bones[11].transform = mth::float4x4::RotationX(rotval * 10.0f);
		bones[12].transform = mth::float4x4::RotationX(rotval * -3.0f);
	}

	DuckAnimation::DuckAnimation(Skeleton& skeleton, mth::Positionf& target) :
		Animation(skeleton, target),
		m_periodTime(0.0f) {}
	DuckAnimation::P DuckAnimation::CreateP(Skeleton& skeleton, mth::Positionf& target)
	{
		return std::make_shared<DuckAnimation>(skeleton, target);
	}
	DuckAnimation::U DuckAnimation::CreateU(Skeleton& skeleton, mth::Positionf& target)
	{
		return std::make_unique<DuckAnimation>(skeleton, target);
	}
	void DuckAnimation::CalculateTransform(float deltaTime)
	{
		m_periodTime += deltaTime;

		float rotval = std::sin(m_periodTime * 10.0f);
		//rotval += 1.0f;
		rotval *= 0.1f;

		MoveGlobal();
		MoveTail(rotval);
		MoveHead(rotval);
		MoveLeftLeg(rotval);
		MoveRightLeg(rotval);
	}
}