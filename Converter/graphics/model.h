#pragma once

#include "graphics.h"
#include "math/linalg.hpp"

namespace Converter
{
	namespace gfx
	{
		struct Vertex
		{
			mth::float3 position;
			mth::float2 texcoord;
			mth::float3 normal;
			mth::float3 tangent;
		};

		class Model
		{
		public:
			SMART_PTR(Model)
		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

		public:
			Model(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount);
			static Model::P CreateP(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount);
			static Model::U CreateU(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount);

			void SetBuffersToRender(Graphics& graphics);
			void RenderGroup(Graphics& graphics, unsigned startIndex, unsigned indexCount);
		};
	}
}