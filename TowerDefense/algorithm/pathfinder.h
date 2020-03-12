#pragma once

#include "pch.h"
#include "common/array2d.hpp"

namespace TowerDefense
{
	namespace alg
	{
		struct Point
		{
			int x, y;
		};

		class PathFinder
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
			PathFinder();
			PathFinder(unsigned width, unsigned height);

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