#include "pathfinder.h"
#include "image.h"
#include <cmath>
#include <iostream>

class LineSection
{
public:
	float point1x, point1y;
	float point2x, point2y;

	LineSection(float p1x, float p1y, float p2x, float p2y) : point1x(p1x), point1y(p1y), point2x(p2x), point2y(p2y) {}

	bool Intersects(LineSection& ls)
	{
		float den = (point1x - point2x) * (ls.point1y - ls.point2y) - (point1y - point2y) * (ls.point1x - ls.point2x);
		if (den < 1e-4f) return false;
		float t = ((point1x - ls.point1x) * (ls.point1y - ls.point2y) - (point1y - ls.point1y) * (ls.point1x - ls.point2x)) / den;
		float u = ((point1x - point2x) * (point1y - ls.point1y) - (point1y - point2y) * (ls.point1x - ls.point2y)) / den;

		if (t < 0.0f || t > 1.0f || u < 0.0f || u > 1.0f)
			return false;
		return true;
	}
	float Distance(float x, float y)
	{
		float num = std::abs((point2y - point1y) * x - (point2x - point1x) * y + point2x * point1y - point2y * point1x);
		float den = std::sqrt((point2y - point1y) * (point2y - point1y) + (point2x - point1x) * (point2x - point1x));
		return num / den;
	}
};

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
	auto Print = [n1, n2, this](float x, float y, float ox, float oy, float th, LineSection& ls) {
		return;
		if (n1 == &m_start || n2 == &m_start)
			std::cout
			<< "x: " << x
			<< ", y: " << y
			<< ", ox: " << ox
			<< ", oy: " << oy
			<< ", th: " << th
			<< ", dst: " << ls.Distance(x + ox, y + oy) << std::endl;
	};

	constexpr float smallDistance = 0.5f;
	constexpr float bigDistance = 1.0f;
	LineSection ls(n1->x, n1->y, n2->x, n2->y);
	int minx = std::fmax(std::floor(n1->x < n2->x ? n1->x : n2->x), 0);
	int miny = std::fmax(std::floor(n1->y < n2->y ? n1->y : n2->y), 0);
	int maxx = std::fmin(std::floor(n1->x > n2->x ? n1->x : n2->x) + 1, m_map.width() - 1);
	int maxy = std::fmin(std::floor(n1->y > n2->y ? n1->y : n2->y) + 1, m_map.height() - 1);
	for (int x = minx; x < maxx; x++)
	{
		for (int y = miny; y < maxy; y++)
		{
			NodeType& nt = m_map(x, y);
			if (nt.type == NodeType::WALL)
			{
				if (ls.Distance(x - 0.5f, y - 0.5f) < smallDistance)
				{
					Print(x, y, -0.5f, -0.5f, smallDistance, ls);
					return false;
				}
				if (ls.Distance(x - 0.5f, y + 0.5f) < smallDistance)
				{
					Print(x, y, -0.5f, +0.5f, smallDistance, ls);
					return false;
				}
				if (ls.Distance(x + 0.5f, y - 0.5f) < smallDistance)
				{
					Print(x, y, +0.5f, -0.5f, smallDistance, ls);
					return false;
				}
				if (ls.Distance(x + 0.5f, y + 0.5f) < smallDistance)
				{
					Print(x, y, +0.5f, +0.5f, smallDistance, ls);
					return false;
				}
				if (ls.Distance(x, y) < bigDistance)
				{
					Print(x, y, 0.0f, 0.0f, bigDistance, ls);
					return false;
				}
				/*if (LineSection(x, y, x + 1, y).Intersects(ls))
					return false;
				if (LineSection(x, y, x, y + 1).Intersects(ls))
					return false;
				if (LineSection(x + 1, y, x + 1, y + 1).Intersects(ls))
					return false;
				if (LineSection(x, y + 1, x + 1, y + 1).Intersects(ls))
					return false;*/
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
					(v2->x - v1->x) * (v2->x - v1->x) +
					(v2->y - v1->y) * (v2->y - v1->y)) });

		NodeType* closestEnd = nullptr;
		float closestEndEst = INFINITY;
		for (NodeType* end : m_ends)
		{
			float est = (end->x - v1->x) * (end->x - v1->x) +
				(end->y - v1->y) * (end->y - v1->y);
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

	for (NodeType* e : m_path)
		std::cout << "x: " << e->x << ", y: " << e->y << std::endl;
	std::cout << std::endl;
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
			m_map(x, y).x = x + 0.5f;
			m_map(x, y).y = y + 0.5f;
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
		m_start.x = x;
		m_start.y = y;
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
