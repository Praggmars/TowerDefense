#pragma once

#include "math/position.hpp"

namespace TowerDefense
{
	namespace gfx
	{
		class Camera :public mth::Position<float>
		{
			mth::float4x4 m_viewMatrix;
			mth::float4x4 m_projectionMatrix;
			float m_screenAspect;
			float m_fieldOfView;
			float m_screenNear;
			float m_screenDepth;

		private:
			void SetProjectionMatrix();

		public:
			Camera(float screenAspect = 16.0f / 9.0f, float fov = mth::PI / 4.0f, float screenNear = 0.1f, float screenDepth = 1000.0f);
			inline static float ToScreenAspect(int width, int height) { return static_cast<float>(width) / static_cast<float>(height); }
			void SetScreenAspect(float screenAspect);
			void SetParams(float screenAspect, float fov, float screenNear = 0.1f, float screenDepth = 1000.0f);

			mth::float3 DirectionFromPoint(mth::float2 point, mth::float2 windowSize);

			void Update();
			mth::float4x4 ViewMatrix();
			mth::float4x4 ProjectionMatrix();
			mth::float4x4 CameraMatrix();
		};
	}
}