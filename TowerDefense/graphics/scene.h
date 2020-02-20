#pragma once

#include "vertexshader.h"
#include "pixelshader.h"
#include "shaderbuffer.h"
#include "entity.h"
#include "camera.h"

/*namespace TowerDefense
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
			Graphics& m_graphics;
			VertexShader::P m_vertexShader;
			ShaderBuffer::P m_vsMatrixBuffer;
			PixelShader::P m_pixelShader;
			ShaderBuffer::P m_psLightBuffer;
			ShaderBuffer::P m_psMaterialBuffer;

			mth::float4x4 m_matrixBuffer[2];
			LightData m_lightData;
			mth::float4 m_bgColor;

		public:
			Scene(Graphics& graphics);
			static Scene::P CreateP(Graphics& graphics);
			static Scene::U CreateU(Graphics& graphics);
			void StartRendering(Camera& camera);
			void RenderEntity(Entity& entity);
			void EndRendering();

			inline void BackgroundColor(mth::float4 color) { m_bgColor = color; }
		};
	}
}*/