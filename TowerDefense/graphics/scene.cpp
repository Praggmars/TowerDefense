#include "pch.h"
#include "scene.h"

/*namespace TowerDefense
{
	namespace gfx
	{
		Scene::Scene(Graphics& graphics) :
			m_graphics(graphics),
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
		void Scene::StartRendering(Camera& camera)
		{
			m_graphics.Clear(static_cast<float*>(m_bgColor));

			m_vertexShader->SetToRender(m_graphics);
			m_vsMatrixBuffer->SetToVertexShader(m_graphics);
			m_pixelShader->SetToRender(m_graphics);
			m_psLightBuffer->SetToPixelShader(m_graphics);
			m_psMaterialBuffer->SetToPixelShader(m_graphics, 1);

			camera.Update();
			m_matrixBuffer[1] = camera.CameraMatrix();

			m_lightData.m_lightPosition = camera.position;
			m_psLightBuffer->WriteBuffer(m_graphics, &m_lightData);
		}
		void Scene::RenderEntity(Entity& entity)
		{
			m_matrixBuffer[0] = entity.WorldMatrix();
			m_vsMatrixBuffer->WriteBuffer(m_graphics, m_matrixBuffer);
			entity.Render(m_graphics, *m_psMaterialBuffer);
		}
		void Scene::EndRendering()
		{
			m_graphics.Present();
		}
	}
}*/