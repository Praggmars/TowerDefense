#include "levelpathfinder.h"

namespace TowerDefense
{
	namespace content
	{
		LevelPathFinder::Node::Node(mth::float2 point) :
			point(point),
			prevNode(nullptr),
			closestEnd(nullptr),
			neighbors(),
			cost(INFINITY),
			estimationToEnd(INFINITY),
			needCheck(false) {}

		bool LevelPathFinder::IsConnectionOk(mth::LineSection2Df ls)
		{
			constexpr float d = 0.5f;
			mth::float2 offsets[] = {
				mth::float2(d, d),
				mth::float2(d, -d),
				mth::float2(-d, -d),
				mth::float2(-d, d)
			};
			for (mth::float2 turret : m_turrets)
			{
				if (mth::LineSection2Df(turret + offsets[0], turret + offsets[1]).Intersects(ls))
					return false;
				if (mth::LineSection2Df(turret + offsets[1], turret + offsets[2]).Intersects(ls))
					return false;
				if (mth::LineSection2Df(turret + offsets[2], turret + offsets[3]).Intersects(ls))
					return false;
				if (mth::LineSection2Df(turret + offsets[3], turret + offsets[0]).Intersects(ls))
					return false;
			}
			return true;
		}
		void LevelPathFinder::EstimateCosts()
		{
			for (Node* n : m_nodes)
			{
				n->estimationToEnd = INFINITY;
				for (Node* e : m_endNodes)
				{
					float est = (n->point - e->point).LengthSquare();
					if (n->estimationToEnd > est)
						n->estimationToEnd = est;
				}
				n->estimationToEnd = std::sqrt(n->estimationToEnd);
			}
		}
		void LevelPathFinder::CreateGraph()
		{
			for (Node* node1 : m_nodes)
			{
				node1->estimationToEnd = INFINITY;
				for (Node* endNode : m_endNodes)
				{					
					float est = (node1->point - endNode->point).LengthSquare();
					if (node1->estimationToEnd > est)
					{
						node1->estimationToEnd = est;
						if (IsConnectionOk(mth::LineSection2Df(node1->point, endNode->point)))
							node1->closestEnd = endNode;
					}
				}
				node1->estimationToEnd = std::sqrt(node1->estimationToEnd);

				for (Node* node2 : m_nodes)
				{
					if (node1 != node2 && IsConnectionOk(mth::LineSection2Df(node1->point, node2->point)))
					{
						float cost = (node1->point, node2->point).Length();
						node1->neighbors.push_back({ node2, cost });
						node2->neighbors.push_back({ node1, cost });
					}
				}
			}
		}
		void LevelPathFinder::SetupPathFinding()
		{
			m_pathPosition = 0;
			for (Node* n : m_nodes)
			{
				n->cost = INFINITY;
				n->needCheck = false;
			}
		}

		void LevelPathFinder::ReconstructPath(Node* endNode)
		{
		}

		void LevelPathFinder::SetStart(mth::float2 position)
		{
			if (m_startNode)
				m_startNode->point = position;
			else
				m_nodes.push_back(m_startNode = new Node(position));
		}
		void LevelPathFinder::AddEnd(mth::float2 position)
		{
			m_endNodes.push_back(new Node(position));
		}
		void LevelPathFinder::AddTurret(mth::float2 position)
		{
			m_turrets.push_back(position);
		}
		void LevelPathFinder::AddCorner(mth::float2 position)
		{
			m_nodes.push_back(new Node(position));
		}
		void LevelPathFinder::Clear()
		{
			m_turrets.clear();
			for (Node* n : m_endNodes)
				delete n;
			m_endNodes.clear();
			for (Node* n : m_nodes)
				delete n;
			m_nodes.clear();
		}
		void LevelPathFinder::StepToNextNode()
		{
			m_pathPosition++;
		}
		mth::float2 LevelPathFinder::NextNodePoint()
		{
			return m_path[m_pathPosition + 1]->point;
		}
		bool LevelPathFinder::FindPath()
		{
			CreateGraph();
			SetupPathFinding();
			Node* currentNode = m_startNode;
			Node* leastCostNode = nullptr;
			float leastCost = INFINITY;

			do
			{
				for (Node::Neighbor& neighbor : currentNode->neighbors)
				{
					Node* nextNode = neighbor.neighbor;
					if (nextNode->closestEnd)
					{
						nextNode->closestEnd->prevNode = nextNode;
						ReconstructPath(nextNode->closestEnd);
						return true;
					}
					float newCost = currentNode->cost + neighbor.cost;
				}
			} while (true);

			return false;
		}
	}
}