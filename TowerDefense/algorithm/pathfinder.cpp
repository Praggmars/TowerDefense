#include "pch.h"
#include "pathfinder.h"

namespace TowerDefense
{
	namespace alg
	{
		void PathFinder::Prepare()
		{
			m_nodes(m_start.x, m_start.y).type = Node::Type::START;
			m_nodes(m_end.x, m_end.y).type = Node::Type::END;
			m_pathLength = -1;

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
						unsigned costEstimate = abs(x - m_end.x) + abs(y - m_end.y) + m_nodes(x, y).cost;
						if (costEstimate < least)
						{
							least = costEstimate;
							best = { x, y };
						}
					}
				}
			}
			return least != UINT32_MAX;
		}
		void PathFinder::RecontsructPath()
		{
			m_pathLength = 2;
			m_path[m_path.size() - 1] = m_end;
			for (Point prev = m_nodes(m_end.x, m_end.y).prev; m_nodes(prev.x, prev.y).type != Node::Type::START; prev = m_nodes(prev.x, prev.y).prev)
			{
				m_nodes(prev.x, prev.y).type = Node::Type::PATH;
				m_path[m_path.size() - (m_pathLength++)] = prev;
			}
			m_path[m_path.size() - m_pathLength] = m_start;
		}
		PathFinder::PathFinder() :
			m_pathLength(-1) {}
		PathFinder::PathFinder(unsigned width, unsigned height) :
			m_nodes(width, height),
			m_path(width* height),
			m_pathLength(-1) {}
		void PathFinder::Resize(unsigned width, unsigned height)
		{
			m_nodes.resize(width, height);
			m_path.resize(width * height);
			m_pathLength = -1;
		}
		bool PathFinder::FindPath(Point start, Point end)
		{
			m_start = start;
			m_end = end;
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
			return &m_path[m_path.size() - m_pathLength];
		}
		Point PathFinder::Path(int index)
		{
			return m_path[m_path.size() - m_pathLength + index];
		}
		int PathFinder::PathLength()
		{
			return m_pathLength;
		}
	}
}