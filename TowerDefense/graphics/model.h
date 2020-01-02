#pragma once

#include "graphics.h"
#include "modelloader.h"

namespace TowerDefense
{
	namespace gfx
	{
		class Model
		{
		public:
			SMART_PTR(Model)

		private:
			unsigned m_vertexCount;
			unsigned m_indexCount;

			struct Group
			{
				unsigned startIndex;
				unsigned indexCount;
			};
			std::vector<Group> m_groups;

			Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

		private:
			void CreateVertexBuffer(Graphics& graphics, Vertex* vertices, unsigned vertexCount);
			void CreateIndexBuffer(Graphics& graphics, unsigned* indices, unsigned indexCount);

		public:
			Model(Graphics& graphics, ModelLoader& loader);
			Model(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned *indices, unsigned indexCount);
			static Model::P CreateP(Graphics& graphics, ModelLoader& loader);
			static Model::P CreateP(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount);
			static Model::U CreateU(Graphics& graphics, ModelLoader& loader);
			static Model::U CreateU(Graphics& graphics, Vertex* vertices, unsigned vertexCount, unsigned* indices, unsigned indexCount);

			inline unsigned GroupCount() { return (unsigned)m_groups.size(); }

			void SetBuffersToRender(Graphics& graphics);
			void RenderGroup(Graphics& graphics, unsigned index);
			void RenderAll(Graphics& graphics);
		};
	}
}