#include "pch.h"
#include "shaderbuffer.h"

namespace TowerDefense
{
	namespace gfx
	{
		ShaderBuffer::ShaderBuffer(Graphics& graphics, unsigned size)
		{
			D3D11_BUFFER_DESC bufferDesc;
			ID3D11Device3* device = graphics.Device3D();
			m_bufferSize = size;

			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth = m_bufferSize;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			ThrowIfFailed(device->CreateBuffer(&bufferDesc, nullptr, &m_buffer));
		}
		ShaderBuffer::P ShaderBuffer::CreateP(Graphics& graphics, unsigned size)
		{
			return std::make_shared<ShaderBuffer>(graphics, size);
		}
		ShaderBuffer::U ShaderBuffer::CreateU(Graphics& graphics, unsigned size)
		{
			return std::make_unique<ShaderBuffer>(graphics, size);
		}
		bool ShaderBuffer::WriteBuffer(Graphics& graphics, void* data)
		{
			return WriteBuffer(graphics, data, m_bufferSize);
		}
		bool ShaderBuffer::WriteBuffer(Graphics& graphics, void* data, unsigned size)
		{
			ID3D11DeviceContext3* context = graphics.Context3D();
			D3D11_MAPPED_SUBRESOURCE resource;
			if (FAILED(context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
				return false;
			memcpy(resource.pData, data, size);
			context->Unmap(m_buffer.Get(), 0);
			return true;
		}
		void ShaderBuffer::SetToVertexShader(Graphics& graphics, unsigned index)
		{
			graphics.Context3D()->VSSetConstantBuffers(index, 1, m_buffer.GetAddressOf());
		}
		void ShaderBuffer::SetToPixelShader(Graphics& graphics, unsigned index)
		{
			graphics.Context3D()->PSSetConstantBuffers(index, 1, m_buffer.GetAddressOf());
		}
	}
}