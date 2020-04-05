#include "pathfinder.h"
#include "image.h"

void PathFinder::FindCorners()
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

bool PathFinder::CanReach(NodeType* n1, NodeType* n2)
{
	constexpr float offset = 0.9f;
	mth::LineSection2Df ls(n1->p, n2->p);
	int minx = std::fmax(std::floor(n1->p.x < n2->p.x ? n1->p.x : n2->p.x) - 1, 0);
	int miny = std::fmax(std::floor(n1->p.y < n2->p.y ? n1->p.y : n2->p.y) - 1, 0);
	int maxx = std::fmin(std::floor(n1->p.x > n2->p.x ? n1->p.x : n2->p.x) + 2, m_map.width() - 1);
	int maxy = std::fmin(std::floor(n1->p.y > n2->p.y ? n1->p.y : n2->p.y) + 2, m_map.height() - 1);
	for (int x = minx; x < maxx; x++)
	{
		for (int y = miny; y < maxy; y++)
		{
			NodeType& nt = m_map(x, y);
			if (nt.type == NodeType::WALL)
			{
				if (mth::LineSection2Df(mth::float2(nt.p.x - offset, nt.p.y - offset), mth::float2(nt.p.x + offset, nt.p.y - offset)).Intersects(ls))
					return false;
				if (mth::LineSection2Df(mth::float2(nt.p.x - offset, nt.p.y - offset), mth::float2(nt.p.x - offset, nt.p.y + offset)).Intersects(ls))
					return false;
				if (mth::LineSection2Df(mth::float2(nt.p.x + offset, nt.p.y + offset), mth::float2(nt.p.x + offset, nt.p.y - offset)).Intersects(ls))
					return false;
				if (mth::LineSection2Df(mth::float2(nt.p.x + offset, nt.p.y + offset), mth::float2(nt.p.x - offset, nt.p.y + offset)).Intersects(ls))
					return false;
			}
		}
	}
	return true;
}

void PathFinder::CreateGraph()
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
					NodeType::Connection{ v2, std::sqrt(
					(v2->p.x - v1->p.x) * (v2->p.x - v1->p.x) +
					(v2->p.y - v1->p.y) * (v2->p.y - v1->p.y)) });

		NodeType* closestEnd = nullptr;
		float closestEndEst = INFINITY;
		for (NodeType* end : m_ends)
		{
			float est = (end->p.x - v1->p.x) * (end->p.x - v1->p.x) +
				(end->p.y - v1->p.y) * (end->p.y - v1->p.y);
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

bool PathFinder::AStar()
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

void PathFinder::ReconstructPath(NodeType* endNode)
{
	while (endNode)
	{
		m_path.insert(m_path.begin(), endNode);
		endNode = endNode->prev;
	}
}

bool PathFinder::Init()
{
	Image mapImage;
	if (!mapImage.LoadFromFile(L"map.png"))
		return false;
	m_map.resize(mapImage.Width(), mapImage.Height());
	for (int x = 0; x < m_map.width(); x++)
	{
		for (int y = 0; y < m_map.height(); y++)
		{
			if (mapImage(x, y) == Color::Green())
				m_map(x, y).type = NodeType::EMPTY;
			else if (mapImage(x, y) == Color::Yellow())
				m_map(x, y).type = NodeType::EMPTY;
			else if (mapImage(x, y) == Color::Blue())
				m_map(x, y).type = NodeType::END;
			else
				m_map(x, y).type = NodeType::WALL;
			m_map(x, y).p.x = x + 0.5f;
			m_map(x, y).p.y = y + 0.5f;
		}
	}
	return true;
}

bool PathFinder::FindPath()
{
	FindCorners();
	CreateGraph();
	return AStar();
}

bool PathFinder::ReplaceStart(float x, float y)
{
	if (x >= 0 && x < m_map.width() &&
		y >= 0 && y < m_map.height())
	{
		m_start.p.x = x;
		m_start.p.y = y;
		return FindPath();
	}
	return false;
}

bool PathFinder::ToggleWall(int x, int y)
{
	if (x >= 0 && x < m_map.width() &&
		y >= 0 && y < m_map.height())
	{
		switch (m_map(x, y).type)
		{
		case NodeType::WALL:
			m_map(x, y).type = NodeType::EMPTY;
			FindPath();
			return true;
		case NodeType::EMPTY:
			m_map(x, y).type = NodeType::WALL;
			FindPath();
			return true;
		}
	}
	return false;
}

bool PathFinder::PutWall(int x, int y)
{
	if (x >= 0 && x < m_map.width() &&
		y >= 0 && y < m_map.height() &&
		m_map(x, y).type == NodeType::EMPTY)
	{
		m_map(x, y).type = NodeType::WALL;
		FindPath();
		return true;
	}
	return false;
}

bool PathFinder::RemoveWall(int x, int y)
{
	if (x >= 0 && x < m_map.width() &&
		y >= 0 && y < m_map.height() &&
		m_map(x, y).type == NodeType::WALL)
	{
		m_map(x, y).type = NodeType::EMPTY;
		FindPath();
		return true;
	}
	return false;
}
