#pragma once

#include "graphics/skeleton.h"
#include "math/position.hpp"

namespace TowerDefense::gfx
{
	class Animation
	{
	public:
		SMART_PTR(Animation)
	protected:
		Skeleton& m_skeleton;
		mth::Positionf& m_target;

	public:
		Animation(Skeleton& skeleton, mth::Positionf& target);

		virtual void CalculateTransform(float deltaTime) = 0;
	};

	class DuckAnimation : public Animation
	{
	public:
		SMART_PTR(DuckAnimation)
	private:
		float m_periodTime;

	private:
		void MoveGlobal();
		void MoveTail(float rotval);
		void MoveHead(float rotval);
		void MoveLeftLeg(float rotval);
		void MoveRightLeg(float rotval);

	public:
		DuckAnimation(Skeleton& skeleton, mth::Positionf& target);
		static DuckAnimation::P CreateP(Skeleton& skeleton, mth::Positionf& target);
		static DuckAnimation::U CreateU(Skeleton& skeleton, mth::Positionf& target);

		virtual void CalculateTransform(float deltaTime) override;
	};
}