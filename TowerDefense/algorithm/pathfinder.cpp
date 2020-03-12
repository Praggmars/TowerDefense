#include "pch.h"
#include "pathfinder.h"

namespace TowerDefense
{
	namespace alg
	{
		void PathFinder::Prepare()
		{
			m_pathLength = -1;
			m_nodes(m_start.x, m_start.y).type = Node::Type::START;
			for (Point end : m_end)
				m_nodes(end.x, end.y).type = Node::Type::END;

			for (Node& n : m_nodes)
			{
				n.cost = UINT32_MAX;
				n.needCheck = false;
			}
			m_nodes(m_start.x, m_start.y).cost = 0;
		}
		bool PathFinder::IsPointOnMap(Point p)
		{
			return p.x >= 0 && p.x < m_nodes.width() && p.y >= 0 && p.y < m_nodes.height();
		}
		bool PathFinder::LeastCostEstimate(Point& best)
		{
			unsigned least = UINT32_MAX;
			for (int y = 0; y < m_nodes.height(); y++)
			{
				for (int x = 0; x < m_nodes.width(); x++)
				{
					if (m_nodes(x, y).needCheck)
					{
						for (Point end : m_end)
						{
							unsigned costEstimate = abs(x - end.x) + abs(y - end.y) + m_nodes(x, y).cost;
							if (costEstimate < least)
							{
								least = costEstimate;
								best = { x, y };
								m_bestEnd = end;
							}
						}
					}
				}
			}
			return least != UINT32_MAX;
		}
		void PathFinder::RecontsructPath()
		{
			int pathPos = static_cast<int>(m_path.size()) - 1;
			m_pathLength = 2;
			m_path[--pathPos] = m_bestEnd;
			for (Point prev = m_nodes(m_bestEnd.x, m_bestEnd.y).prev; m_nodes(prev.x, prev.y).type != Node::Type::START; prev = m_nodes(prev.x, prev.y).prev)
			{
				m_nodes(prev.x, prev.y).type = Node::Type::PATH;
				m_path[--pathPos] = prev;
				m_pathLength++;
			}
			m_path[pathPos] = m_start;
		}
		PathFinder::PathFinder() :
			m_pathLength(-1) {}
		PathFinder::PathFinder(unsigned width, unsigned height) :
			m_nodes(width, height),
			m_path(width * height),
			m_pathLength(-1) {}
		void PathFinder::Resize(unsigned width, unsigned height)
		{
			m_nodes.resize(width, height);
			m_path.resize(width * height);
			m_pathLength = -1;
		}
		bool PathFinder::FindPath(Point start, Point end)
		{
			return FindPath(start, &end, 1);
		}
		bool PathFinder::FindPath(Point start, Point* end, unsigned endCount)
		{
			m_start = start;
			m_end.resize(endCount);
			for (unsigned i = 0; i < endCount; i++)
				m_end[i] = end[i];
			Prepare();
			Point current = m_start;
			do
			{
				m_nodes(current.x, current.y).needCheck = false;
				Point neighborPoints[]{
					Point{current.x - 1, current.y},
					Point{current.x + 1, current.y},
					Point{current.x, current.y - 1},
					Point{current.x, current.y + 1} };
				for (Point neighborPoint : neighborPoints)
				{
					if (IsPointOnMap(neighborPoint))
					{
						Node& neighbor = m_nodes(neighborPoint.x, neighborPoint.y);
						if (neighbor.type == Node::Type::END)
						{
							neighbor.prev = current;
							RecontsructPath();
							return true;
						}
						if (neighbor.type == Node::Type::EMPTY)
						{
							unsigned cost = m_nodes(current.x, current.y).cost + 1;
							if (neighbor.cost > cost)
							{
								neighbor.cost = cost;
								neighbor.prev = current;
								neighbor.needCheck = true;
							}
						}
					}
				}
			} while (LeastCostEstimate(current));
			return false;
		}
		void PathFinder::Block(int x, int y)
		{
			m_nodes(x, y).type = Node::Type::BLOCK;
		}
		void PathFinder::Unblock(int x, int y)
		{
			m_nodes(x, y).type = Node::Type::EMPTY;
		}
		void PathFinder::RemoveBlocks()
		{
			for (auto& n : m_nodes)
				n.type = Node::Type::EMPTY;
		}
		Point* PathFinder::Path()
		{
			if (m_pathLength < 0)
				return nullptr;
			return &m_path[m_path.size() - m_pathLength - 1];
		}
		Point PathFinder::Path(int index)
		{
			return m_path[m_path.size() - m_pathLength - 1 + index];
		}
		int PathFinder::PathLength()
		{
			return m_pathLength;
		}
	}
}