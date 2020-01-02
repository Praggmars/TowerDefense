#pragma once

#include "vertexshader.h"
#include "pixelshader.h"
#include "shaderbuffer.h"
#include "entity.h"
#include "camera.h"

namespace TowerDefense
{
	namespace gfx
	{
		class Scene
		{
			struct LightData
			{
				mth::float4 m_lightColor;
				mth::float3 m_lightPosition;
				float m_ambientLight;
			};

		public:
			SMART_PTR(Scene)

		private:
			VertexShader::P m_vertexShader;
			ShaderBuffer::P m_vsMatrixBuffer;
			PixelShader::P m_pixelShader;
			ShaderBuffer::P m_psLightBuffer;
			ShaderBuffer::P m_psMaterialBuffer;

			Camera m_camera;
			LightData m_lightData;
			mth::float4 m_bgColor;

			std::vector<Entity::P> m_entities;

		public:
			Scene(Graphics& graphics);
			static Scene::P CreateP(Graphics& graphics);
			static Scene::U CreateU(Graphics& graphics);

			void AddEntity(Entity::P entity);
			void RemoveEntity(Entity::P entity);
			void Render(Graphics& graphics);

			inline std::vector<Entity::P>& Entities() { return m_entities; }
			inline void BackgroundColor(mth::float4 color) { m_bgColor = color; }
			inline Camera& Camera() { return m_camera; }
		};
	}
}