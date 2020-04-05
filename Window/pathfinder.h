#pragma once

#include "array2d.hpp"
#include "math/geometry2d.hpp"

struct NodeType
{
	enum Types
	{
		WALL,
		START,
		END,
		EMPTY,
		CORNER
	};
	struct Connection
	{
		NodeType* neighbor;
		float cost;
	};

	Types type;
	mth::float2 p;
	float estToEnd;
	float cost;
	NodeType* prev;
	std::vector<Connection> neighbors;
	bool needsCheck;
	bool isCorner;
};

class PathFinder
{
	Array2D<NodeType> m_map;
	NodeType m_start;
	std::vector<NodeType*> m_ends;
	std::vector<NodeType*> m_graph;
	std::vector<NodeType*> m_path;

private:
	void FindCorners();
	bool CanReach(NodeType* n1, NodeType* n2);
	void CreateGraph();
	bool AStar();
	void ReconstructPath(NodeType* endNode);

public:
	bool Init();

	bool FindPath();

	bool ReplaceStart(float x, float y);
	bool ToggleWall(int x, int y);
	bool PutWall(int x, int y);
	bool RemoveWall(int x, int y);

	inline int Width() { return m_map.width(); }
	inline int Height() { return m_map.height(); }
	inline NodeType& Node(int x, int y) { return m_map(x, y); }
	inline NodeType* StartNode() { return &m_start; }
	inline std::vector<NodeType*>& Graph() { return m_graph; }
	inline std::vector<NodeType*>& Path() { return m_path; }
};