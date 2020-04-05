#pragma once

#include "pch.h"

namespace TowerDefense
{
	template <typename T>
	class FixArray
	{
		std::vector<T> m_array;
		size_t m_size;

	public:
		T& operator[](size_t index) { return m_array[index]; }
		void resize(size_t size)
		{
			m_array.resize(size);
		}

		auto begin() { return m_array.begin(); }
		auto end() { return m_array.begin() + m_size; }
	};
}