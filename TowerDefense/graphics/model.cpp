#include "pch.h"
#include "common/resourceloadingmanager.h"

namespace TowerDefense
{
	namespace gfx
	{
		void Model::CreateVertexBuffer(Graphics& graphics, Vertex* vertices, unsigned vertexCount)
		{
			D3D11_BUFFER_DESC bufferDesc{};
			D3D11_SUBRESOURCE_DATA subResourceData{};

			m_vertexCount = vertexCount;

			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = m_vertexCount * sizeof(Vertex);
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subResourceData.pSysMem = vertices;

			HRESULT hr = graphics.Device3D()->CreateBuffer(&bufferDesc, &subResourceData, &m_vertexBuffer);
			if (FAILED(hr))
				throw std::exception("Failed to create vertex buffer");
		}
		void Model::CreateIndexBuffer(Graphics& graphics, unsigned* indices, unsigned indexCount)
		{
			D3D11_BUFFER_DESC bufferDesc{};
			D3D11_SUBRESOURCE_DATA subResourceData{};

			m_indexCount = indexCount;

			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = m_indexCount * sizeof(UINT);
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			subResourceData.pSysMem = indices;

			HRESULT hr = graphics.Device3D()->CreateBuffer(&bufferDesc, &subResourceData, &m_indexBuffer);
			if (FAILED(hr))
				throw std::exception("Failed to create index buffer");
		}
		Model::Model(Graphics& graphics, ModelLoader& loader)
		{
			CreateVertexBuffer(graphics, loader.Vertices().data(), loader.Vertices().size());
			CreateIndexBuffer(graphics, loader.Indices().data(), loader.Indices().size());

			for (UINT i = 0; i < loader.VertexGroups().size(); i++)
			{
				VertexGroup vg = loader.VertexGroups()[i];
				m_groups.push_back({ vg.startIndex, vg.indexCount });
			}
		}
		Model::Model(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount)
		{
			CreateVertexBuffer(graphics, vertices, vertexCount);
			CreateIndexBuffer(graphics, indices, indexCount);

			m_groups.push_back({ 0, indexCount });
		}
		Model::P Model::CreateP(Graphics& graphics, ModelLoader& loader)
		{
			if (loader.Filename()[0] == '\0')
				return std::make_shared<Model>(graphics, loader);
			auto model = hlp::ResourceLoadingManager::Instance().FindModel(loader.Filename());
			if (!model)
			{
				model = std::make_shared<Model>(graphics, loader);
				hlp::ResourceLoadingManager::Instance().AddModel(model, loader.Filename());
			}
			return model;
		}
		Model::P Model::CreateP(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount)
		{
			return std::make_shared<Model>(graphics, vertices, vertexCount, indices, indexCount);
		}
		Model::U Model::CreateU(Graphics& graphics, ModelLoader& loader)
		{
			return std::make_unique<Model>(graphics, loader);
		}
		Model::U Model::CreateU(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount)
		{
			return std::make_unique<Model>(graphics, vertices, vertexCount, indices, indexCount);
		}
		void Model::SetBuffersToRender(Graphics& graphics)
		{
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			ID3D11DeviceContext3* context = graphics.Context3D();

			context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
			context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		}
		void Model::RenderGroup(Graphics& graphics, unsigned index)
		{
			graphics.Context3D()->DrawIndexed(m_groups[index].indexCount, m_groups[index].startIndex, 0);
		}
		void Model::RenderAll(Graphics& graphics)
		{
			SetBuffersToRender(graphics);
			graphics.Context3D()->DrawIndexed(m_indexCount, 0, 0);
		}
	}
}