#include "pch.h"
#include "model.h"

namespace Converter
{
	namespace gfx
	{
		Model::Model(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount)
		{
			HRESULT hr;
			D3D11_BUFFER_DESC bufferDesc{};
			D3D11_SUBRESOURCE_DATA subResourceData{};
			ID3D11Device* device = graphics.Device3D();

			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = vertexCount * sizeof(Vertex);
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			subResourceData.pSysMem = vertices;

			hr = device->CreateBuffer(&bufferDesc, &subResourceData, &m_vertexBuffer);
			if (FAILED(hr))
				throw std::exception("Failed to create vertex buffer");

			bufferDesc.ByteWidth = indexCount * sizeof(unsigned);
			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			subResourceData.pSysMem = indices;

			hr = device->CreateBuffer(&bufferDesc, &subResourceData, &m_indexBuffer);
			if (FAILED(hr))
				throw std::exception("Failed to create index buffer");
		}
		Model::P Model::CreateP(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount)
		{
			return std::make_shared<Model>(graphics, vertices, vertexCount, indices, indexCount);
		}
		Model::U Model::CreateU(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount)
		{
			return std::make_unique<Model>(graphics, vertices, vertexCount, indices, indexCount);
		}
		void Model::SetBuffersToRender(Graphics& graphics)
		{
			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			ID3D11DeviceContext* context = graphics.Context3D();

			context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
			context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		}
		void Model::RenderGroup(Graphics& graphics, unsigned startIndex, unsigned indexCount)
		{
			graphics.Context3D()->DrawIndexed(indexCount, startIndex, 0);
		}
	}
}