#pragma once

#include "pch.h"
#include "common/array2d.hpp"
#include "math/linalg.hpp"

namespace TowerDefense
{
	namespace alg
	{
		struct Point
		{
			int x, y;
		};

		inline mth::float2 ToFloat2(Point p)
		{
			return mth::float2(
				static_cast<float>(p.x),
				static_cast<float>(p.y)
				);
		}
		inline Point ToPoint(mth::float2 p)
		{
			return Point{
				static_cast<int>(p.x),
				static_cast<int>(p.y)
			};
		}

		class PathFinderGrid
		{
			struct Node
			{
				enum class Type { EMPTY, BLOCK, PATH, START, END };
				Type type;
				unsigned cost;
				Point prev;
				bool needCheck;
			};

			Array2D<Node> m_nodes;
			std::vector<Point> m_path;
			Point m_start;
			std::vector<Point> m_end;
			Point m_bestEnd;
			int m_pathLength;

		private:
			void Prepare();
			bool IsPointOnMap(Point p);
			bool LeastCostEstimate(Point& best);
			void RecontsructPath();

		public:
			PathFinderGrid();
			PathFinderGrid(unsigned width, unsigned height);

			void Resize(unsigned width, unsigned height);

			bool FindPath(Point start, Point end);
			bool FindPath(Point start, Point* end, unsigned endCount);
			void Block(int x, int y);
			void Unblock(int x, int y);
			void RemoveBlocks();

			Point* Path();
			Point Path(int index);
			int PathLength();
		};
	}
}