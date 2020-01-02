#pragma once

#include "graphics.h"

namespace TowerDefense
{
	namespace gfx
	{
		class ShaderBuffer
		{
		public:
			SMART_PTR(ShaderBuffer)

		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
			unsigned m_bufferSize;

		public:
			ShaderBuffer(Graphics& graphics, unsigned size);
			static ShaderBuffer::P CreateP(Graphics& graphics, unsigned size);
			static ShaderBuffer::U CreateU(Graphics& graphics, unsigned size);

			bool WriteBuffer(Graphics& graphics, void* data);
			bool WriteBuffer(Graphics& graphics, void* data, unsigned size);
			void SetToVertexShader(Graphics& graphics, unsigned index = 0);
			void SetToPixelShader(Graphics& graphics, unsigned index = 0);

		};
	}
}