#include "pathfinderdiag.h"

namespace TowerDefense
{
	namespace alg
	{
		void PathFinderDiag::FindCorners()
		{
			for (int x = 1; x < m_map.width() - 1; x++)
			{
				for (int y = 1; y < m_map.height() - 1; y++)
				{
					m_map(x, y).isCorner = ((m_map(x, y).type != NodeType::WALL) && (
						(m_map(x - 1, y - 1).type == NodeType::WALL && m_map(x, y - 1).type != NodeType::WALL && m_map(x - 1, y).type != NodeType::WALL) ||
						(m_map(x - 1, y + 1).type == NodeType::WALL && m_map(x, y + 1).type != NodeType::WALL && m_map(x - 1, y).type != NodeType::WALL) ||
						(m_map(x + 1, y - 1).type == NodeType::WALL && m_map(x, y - 1).type != NodeType::WALL && m_map(x + 1, y).type != NodeType::WALL) ||
						(m_map(x + 1, y + 1).type == NodeType::WALL && m_map(x, y + 1).type != NodeType::WALL && m_map(x + 1, y).type != NodeType::WALL)));
				}
			}
		}

		bool PathFinderDiag::CanReach(NodeType* n1, NodeType* n2)
		{
			constexpr float offset = 0.9f;
			mth::LineSection2Df ls(n1->position, n2->position);
			int minx = std::fmax(std::floor(n1->position.x < n2->position.x ? n1->position.x : n2->position.x) - 1, 0);
			int miny = std::fmax(std::floor(n1->position.y < n2->position.y ? n1->position.y : n2->position.y) - 1, 0);
			int maxx = std::fmin(std::floor(n1->position.x > n2->position.x ? n1->position.x : n2->position.x) + 2, m_map.width() - 1);
			int maxy = std::fmin(std::floor(n1->position.y > n2->position.y ? n1->position.y : n2->position.y) + 2, m_map.height() - 1);
			for (int x = minx; x < maxx; x++)
			{
				for (int y = miny; y < maxy; y++)
				{
					NodeType& nt = m_map(x, y);
					if (nt.type == NodeType::WALL)
					{
						if (mth::LineSection2Df(mth::float2(nt.position.x - offset, nt.position.y - offset), mth::float2(nt.position.x + offset, nt.position.y - offset)).Intersects(ls))
							return false;
						if (mth::LineSection2Df(mth::float2(nt.position.x - offset, nt.position.y - offset), mth::float2(nt.position.x - offset, nt.position.y + offset)).Intersects(ls))
							return false;
						if (mth::LineSection2Df(mth::float2(nt.position.x + offset, nt.position.y + offset), mth::float2(nt.position.x + offset, nt.position.y - offset)).Intersects(ls))
							return false;
						if (mth::LineSection2Df(mth::float2(nt.position.x + offset, nt.position.y + offset), mth::float2(nt.position.x - offset, nt.position.y + offset)).Intersects(ls))
							return false;
					}
				}
			}
			return true;
		}

		void PathFinderDiag::CreateGraph()
		{
			m_path.clear();
			m_ends.clear();
			m_graph.clear();
			for (NodeType& n : m_map)
			{
				n.estToEnd = INFINITY;
				n.neighbors.clear();
				n.cost = INFINITY;
				n.prev = nullptr;
				n.needsCheck = false;
			}
			for (int x = 1; x < m_map.width() - 1; x++)
			{
				for (int y = 1; y < m_map.height() - 1; y++)
				{
					NodeType& n = m_map(x, y);
					if (n.type == NodeType::END)
					{
						n.estToEnd = 0.0f;
						m_ends.push_back(&n);
						//m_graph.push_back(&n);
					}
					else if (n.isCorner)
					{
						m_graph.push_back(&n);
					}
				}
			}

			m_start.cost = 0.0f;
			m_start.estToEnd = INFINITY;
			m_start.neighbors.clear();
			m_start.prev = nullptr;
			m_start.needsCheck = false;
			m_start.type = NodeType::START;
			m_graph.push_back(&m_start);

			for (NodeType* v1 : m_graph)
			{
				for (NodeType* v2 : m_graph)
					if ((v1 != v2) && CanReach(v1, v2))
						v1->neighbors.push_back(
							NodeType::Connection{ v2, (v2->position-v1->position).Length() });

				NodeType* closestEnd = nullptr;
				float closestEndEst = INFINITY;
				for (NodeType* end : m_ends)
				{
					float est = (end->position - v1->position).LengthSquare();
					if (v1->estToEnd > est)
						v1->estToEnd = est;
					if ((closestEndEst > est) && CanReach(v1, end))
					{
						closestEndEst = est;
						closestEnd = end;
					}
				}
				v1->estToEnd = std::sqrt(v1->estToEnd);
				if (closestEnd)
					v1->neighbors.push_back(NodeType::Connection{ closestEnd, std::sqrt(closestEndEst) });
			}
		}

		bool PathFinderDiag::AStar()
		{
			NodeType* currentNode = &m_start;
			do
			{
				currentNode->needsCheck = false;
				if (currentNode->type == NodeType::END)
				{
					ReconstructPath(currentNode);
					return true;
				}
				if (currentNode->neighbors.empty()) return false;
				for (NodeType::Connection& conn : currentNode->neighbors)
				{
					NodeType* neighbor = conn.neighbor;
					float newCost = currentNode->cost + conn.cost;
					if (neighbor->cost > newCost)
					{
						neighbor->prev = currentNode;
						neighbor->cost = newCost;
						neighbor->needsCheck = true;
					}
				}

				currentNode = nullptr;
				float leastCost = INFINITY;
				for (NodeType* n : m_graph)
				{
					if (n->needsCheck && n->cost + n->estToEnd < leastCost)
					{
						leastCost = n->cost + n->estToEnd;
						currentNode = n;
					}
				}
				for (NodeType* n : m_ends)
				{
					if (n->needsCheck && n->cost + n->estToEnd < leastCost)
					{
						leastCost = n->cost + n->estToEnd;
						currentNode = n;
					}
				}
			} while (currentNode);
			return false;
		}

		void PathFinderDiag::ReconstructPath(NodeType* endNode)
		{
			while (endNode)
			{
				m_path.insert(m_path.begin(), endNode);
				endNode = endNode->prev;
			}
		}

		bool PathFinderDiag::FindPath()
		{
			FindCorners();
			CreateGraph();
			return AStar();
		}

		void PathFinderDiag::ReplaceStart(float x, float y)
		{
			if (x >= 0 && x < m_map.width() &&
				y >= 0 && y < m_map.height() &&
				m_map(x, y).type == NodeType::EMPTY)
			{
				m_start.position.x = x;
				m_start.position.y = y;
			}
		}

		void PathFinderDiag::ToggleWall(int x, int y)
		{
			if (x >= 0 && x < m_map.width() &&
				y >= 0 && y < m_map.height())
			{
				switch (m_map(x, y).type)
				{
				case NodeType::WALL:
					m_map(x, y).type = NodeType::EMPTY;
					break;
				case NodeType::EMPTY:
					m_map(x, y).type = NodeType::WALL;
					break;
				}
			}
		}
		void PathFinderDiag::PutWall(int x, int y)
		{
			if (x >= 0 && x < m_map.width() &&
				y >= 0 && y < m_map.height() &&
				m_map(x, y).type == NodeType::EMPTY)
			{
				m_map(x, y).type = NodeType::WALL;
			}
		}
		void PathFinderDiag::RemoveWall(int x, int y)
		{
			if (x >= 0 && x < m_map.width() &&
				y >= 0 && y < m_map.height() &&
				m_map(x, y).type == NodeType::WALL)
			{
				m_map(x, y).type = NodeType::EMPTY;
			}
		}
		void PathFinderDiag::AddEnd(int x, int y)
		{
			if (x >= 0 && x < m_map.width() &&
				y >= 0 && y < m_map.height())
			{
				m_map(x, y).type = NodeType::END;
			}
		}
		void PathFinderDiag::Resize(int width, int height)
		{
			m_map.resize(static_cast<size_t>(width), static_cast<size_t>(height));
			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					m_map(x, y).position = mth::float2(x, y);
				}
			}
		}
	}
}