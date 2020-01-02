#pragma once

#include "graphics.h"

namespace TowerDefense
{
	namespace gfx
	{
		class VertexShader
		{
		public:
			SMART_PTR(VertexShader)

		private:
			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
			Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

		public:
			VertexShader(Graphics& graphics);
			static VertexShader::P CreateP(Graphics& graphics);
			static VertexShader::U CreateU(Graphics& graphics);

			void SetToRender(Graphics& graphics);
		};
	}
}