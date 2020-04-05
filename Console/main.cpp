#include <iostream>
#include <sstream>ss
#include <vector>

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
	inline size_t width() { return m_width; }
	inline size_t height() { return m_height; }

	inline auto begin() { return m_array.begin(); }
	inline auto end() { return m_array.end(); }
};

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
						for (Point n = neighbor.prev; node(n).type != Node::Type::START; n = node(n).prev)
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

struct Int
{
	int i;

	Int() :i(0) { std::cout << "new Int\n"; }
	Int(int i) :i(i) { std::cout << "new Int\n"; }
	Int(const Int& i) :i(i.i) { std::cout << "new Int\n"; }
	Int(Int&& i) noexcept :i(i.i) { std::cout << "new Int\n"; }
	~Int() { std::cout << "delete Int\n"; }

	Int& operator=(int i) { this->i = i; return *this; }
	Int& operator=(const Int& i) { this->i = i.i; return *this; }
	operator int() { return i; }
};

Array2D<Int> CreateArray()
{
	Array2D<Int> a(3, 3);
	int counter = 0;
	for (auto& i : a)
		i = counter++;
	return a;
}

bool ScanStream(std::wistream& s, const wchar_t* expected)
{
	wchar_t sch, ech; //stream char, expected char
	size_t extPos = 0;
	do
	{
		ech = expected[extPos++];
		if (ech == 0) return true;
		s >> sch;
	} while (!s.eof() && ech == sch);
	return false;
}

void Swap(int& a, int& b)
{
	int tmp = a;
	a = b;
	b = tmp;
}
void PrintArray(int* a, int size)
{
	for (int i = 0; i < size; i++)
		std::cout << a[i] << ' ';
	std::cout << std::endl;
}
void Shuffle(int* a, int size)
{
	for (int i = 0; i < size; i++)
		Swap(a[i], a[rand() % size]);
}
void QuickSort(int* a, int size)
{
	int pivot = a[size / 2];
	int i = 0;
	int  j = size - 1;
	while (i <= j)
	{
		while (a[i] < pivot) i++;
		while (a[j] > pivot) j--;
		if (i <= j)
		{
			Swap(a[i], a[j]);
			i++;
			j--;
		}
	}
	if (j > 0)
		QuickSort(a, i);
	if (size - i > 0)
		QuickSort(a + i, size - i);
}
void ShufflePrintSortPrint(int* a, int size)
{
	Shuffle(a, size);
	PrintArray(a, size);
	QuickSort(a, size);
	PrintArray(a, size);
	std::cout << std::endl;
}

int main()
{
	int a[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int size = sizeof(a) / sizeof(*a);

	ShufflePrintSortPrint(a, size);
	ShufflePrintSortPrint(a, size);
	ShufflePrintSortPrint(a, size);

	std::cin.get();
	return 0;
}