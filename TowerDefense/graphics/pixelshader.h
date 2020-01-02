#pragma once

#include "graphics.h"

namespace TowerDefense
{
	namespace gfx
	{
		class PixelShader
		{
		public:
			SMART_PTR(PixelShader)

		private:
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;

		public:
			PixelShader(Graphics& graphics);
			static PixelShader::P CreateP(Graphics& graphics);
			static PixelShader::U CreateU(Graphics& graphics);

			void SetToRender(Graphics& graphics);
		};
	}
}