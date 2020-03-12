#pragma once

#include "pch.h"
#include "math/geometry2d.hpp"
#include "common/array2d.hpp"

namespace TowerDefense
{
	namespace content
	{
		class LevelPathFinder
		{
			struct Node
			{
				struct Neighbor
				{
					Node* neighbor;
					float cost;
				};

				mth::float2 point;
				Node* prevNode;
				Node* closestEnd;
				std::vector<Neighbor> neighbors;
				float cost;
				float estimationToEnd;
				bool needCheck;

				Node(mth::float2 point);
			};

			Node* m_startNode;
			std::vector<Node*> m_endNodes;
			std::vector<mth::float2> m_turrets;
			std::vector<Node*> m_nodes;

			size_t m_pathPosition;
			std::vector<Node*> m_path;

		private:
			bool IsConnectionOk(mth::LineSection2Df ls);
			void EstimateCosts();
			void CreateGraph();

			void SetupPathFinding();
			void ReconstructPath(Node* endNode);

		public:
			void SetStart(mth::float2 position);
			void AddEnd(mth::float2 position);
			void AddTurret(mth::float2 position);
			void AddCorner(mth::float2 position);
			void Clear();

			void StepToNextNode();
			mth::float2 NextNodePoint();

			bool FindPath();
		};
	}
}