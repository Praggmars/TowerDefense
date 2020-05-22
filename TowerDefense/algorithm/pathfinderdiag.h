#pragma once

#include "pch.h"
#include "common/array2d.hpp"
#include "math/geometry2d.hpp"

namespace TowerDefense
{
	namespace alg
	{
		struct NodeType
		{
			enum Types
			{
				WALL,
				START,
				END,
				EMPTY
			};
			struct Connection
			{
				NodeType* neighbor;
				float cost;
			};

			Types type;
			mth::float2 position;
			float estToEnd;
			float cost;
			NodeType* prev;
			std::vector<Connection> neighbors;
			bool needsCheck;
			bool isCorner;
		};

		class PathFinderDiag
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
			bool FindPath();

			void ReplaceStart(float x, float y);
			void ToggleWall(int x, int y);
			void PutWall(int x, int y);
			void RemoveWall(int x, int y);
			void AddEnd(int x, int y);

			void Resize(int width, int height);

			inline int Width() { return m_map.width(); }
			inline int Height() { return m_map.height(); }
			inline NodeType& Node(int x, int y) { return m_map(x, y); }
			inline NodeType* StartNode() { return &m_start; }
			inline std::vector<NodeType*>& Graph() { return m_graph; }
			inline std::vector<NodeType*>& Path() { return m_path; }
		};
	}
}