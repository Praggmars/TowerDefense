#include <iostream>

struct Point
{
	int x, y;
};

template <int W, int H>
class Area
{
	struct Node
	{
		enum class Type : char
		{
			BLOCK = '#',
			PATH = '.',
			EMPTY = ' ',
			START = 'S',
			END = 'E'
		};
		Type type;
		unsigned cost;
		Point prev;
		bool needCheck;
	};

	Node nodes[W * H];
	Point m_start;
	Point m_end;

private:
	inline Node& node(int x, int y) { return nodes[y * W + x]; }
	inline Node& node(Point p) { return nodes[p.y * W + p.x]; }
	bool LeastCostEstimate(Point& best)
	{
		unsigned least = UINT32_MAX;
		for (int y = 0; y < H; y++)
		{
			for (int x = 0; x < W; x++)
			{
				Node& n = node(x, y);
				if (n.needCheck)
				{
					Point p{ x, y };
					int estimate = abs(p.x - m_end.x) + abs(p.y - m_end.y) + n.cost;
					if (estimate < least)
					{
						least = estimate;
						best = p;
					}
				}
			}
		}
		return least != UINT32_MAX;
	}

public:

	Area() :nodes{}
	{
		for (int i = 0; i < W * H; i++)
			nodes[i].type = rand() & 3 ? Node::Type::EMPTY : Node::Type::BLOCK;
	}

	void FindPath(Point start, Point end)
	{
		node(m_start = start).type = Node::Type::START;
		node(m_end = end).type = Node::Type::END;

		for (Node& n : nodes)
		{
			n.cost = UINT32_MAX;
			n.needCheck = false;
		}

		node(m_start).cost = 0;
		Point current = m_start;
		do
		{
			node(current).needCheck = false;
			Point points[]{
				{ current.x - 1, current.y },
				{ current.x + 1, current.y },
				{ current.x, current.y - 1 },
				{ current.x, current.y + 1 } };
			for (Point p : points)
			{
				if (p.x >= 0 && p.x < W && p.y >= 0 && p.y < H)
				{
					Node& neighbor = node(p);
					if (neighbor.type == Node::Type::END)
					{
						neighbor.prev = current;
						Point n = neighbor.prev;
						for (Point n = neighbor.prev;  node(n).type != Node::Type::START; n = node(n).prev)
							node(n).type = Node::Type::PATH;
						return;
					}
					if (neighbor.type == Node::Type::EMPTY)
					{
						if (neighbor.cost > node(current).cost + 1)
						{
							neighbor.cost = node(current).cost + 1;
							neighbor.prev = current;
							neighbor.needCheck = true;
						}
					}
				}
			}
		} while (LeastCostEstimate(current));
	}

	void Print()
	{
		for (int x = 0; x < W + 2; x++)
			std::cout << static_cast<char>(Node::Type::BLOCK);
		std::cout << '\n';
		for (int y = 0; y < H; y++)
		{
			std::cout << static_cast<char>(Node::Type::BLOCK);
			for (int x = 0; x < W; x++)
				std::cout << static_cast<char>(node(x, y).type);
			std::cout << static_cast<char>(Node::Type::BLOCK) << '\n';
		}
		for (int x = 0; x < W + 2; x++)
			std::cout << static_cast<char>(Node::Type::BLOCK);
		std::cout << std::endl;
	}
};

int main()
{
	Area<10, 10> area;
	area.FindPath({ 5, 2 }, { 2, 9 });
	area.Print();
	std::cin.get();
	return 0;
}