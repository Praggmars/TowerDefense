#pragma once

#include "graphics/entity.h"

namespace TowerDefense
{
	namespace content
	{
		class Player
		{
		public:
			SMART_PTR(Player)
		private:
			unsigned m_money;
			unsigned m_health;

		public:
		};
	}
}