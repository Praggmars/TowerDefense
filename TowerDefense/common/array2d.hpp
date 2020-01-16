#pragma once

#include "pch.h"

namespace TowerDefense
{
	template <typename T>
	class Array2D
	{
		std::vector<T> m_array;
		size_t m_width, m_height;

	public:
		Array2D() :
			m_width(0),
			m_height(0) {}
		Array2D(size_t width, size_t height) :
			m_array(width* height),
			m_width(width),
			m_height(height) {}
		Array2D(const Array2D& array2d) :
			m_array(array2d.m_array),
			m_width(array2d.m_width),
			m_height(array2d.m_height) {}
		Array2D(Array2D&& array2d) noexcept :
			m_array(std::move(array2d.m_array)),
			m_width(array2d.m_width),
			m_height(array2d.m_height) {}
		void resize(size_t width, size_t height)
		{
			m_width = width;
			m_height = height;
			m_array.resize(width * height);
		}
		void clear()
		{
			m_width = 0;
			m_height = 0;
			m_array.clear();
		}
		Array2D& operator=(const Array2D& array2d)
		{
			m_array = array2d.m_array;
			m_width = array2d.m_width;
			m_height = array2d.m_height;
			return *this;
		}
		Array2D& operator=(Array2D&& array2d)
		{
			m_array = std::move(array2d.m_array);
			m_width = array2d.m_width;
			m_height = array2d.m_height;
			return *this;
		}
		inline T& operator()(size_t x, size_t y)
		{
			return m_array[y * m_width + x];
		}
		inline T* data() { return m_array.data(); }
		inline size_t width() { return m_width; }
		inline size_t height() { return m_height; }

		inline auto begin() { return m_array.begin(); }
		inline auto end() { return m_array.end(); }
	};
}