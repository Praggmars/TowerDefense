#include "pch.h"
#include "scene.h"

namespace TowerDefense
{
	namespace gfx
	{
		Scene::Scene(Graphics& graphics) :
			m_vertexShader(VertexShader::CreateP(graphics)),
			m_vsMatrixBuffer(ShaderBuffer::CreateP(graphics, sizeof(mth::float4x4) * 2)),
			m_pixelShader(PixelShader::CreateP(graphics)),
			m_psLightBuffer(ShaderBuffer::CreateP(graphics, sizeof(LightData))),
			m_psMaterialBuffer(ShaderBuffer::CreateP(graphics, sizeof(MaterialBuffer))),
			m_lightData{ mth::float4(1.0f), mth::float3(), 0.5f } {}
		Scene::P Scene::CreateP(Graphics& graphics)
		{
			return std::make_shared<Scene>(graphics);
		}
		Scene::U Scene::CreateU(Graphics& graphics)
		{
			return std::make_unique<Scene>(graphics);
		}
		void Scene::AddEntity(Entity::P entity)
		{
			m_entities.push_back(entity);
		}
		void Scene::RemoveEntity(Entity::P entity)
		{
			for (auto e = m_entities.begin(); e != m_entities.end(); e++)
			{
				if (*e == entity)
				{
					m_entities.erase(e);
					return;
				}
			}
		}
		void Scene::Render(Graphics& graphics)
		{
			graphics.Clear(static_cast<float*>(m_bgColor));

			m_vertexShader->SetToRender(graphics);
			m_vsMatrixBuffer->SetToVertexShader(graphics);
			m_pixelShader->SetToRender(graphics);
			m_psLightBuffer->SetToPixelShader(graphics);
			m_psMaterialBuffer->SetToPixelShader(graphics, 1);

			mth::float4x4 matrixBuffer[2];
			m_camera.Update();
			matrixBuffer[1] = m_camera.CameraMatrix();

			m_lightData.m_lightPosition = m_camera.position;
			m_psLightBuffer->WriteBuffer(graphics, &m_lightData);
			
			for (auto& e : m_entities)
			{
				matrixBuffer[0] = e->WorldMatrix();
				m_vsMatrixBuffer->WriteBuffer(graphics, matrixBuffer);
				e->Render(graphics, *m_psMaterialBuffer);
			}

			graphics.Present();
		}
	}
}