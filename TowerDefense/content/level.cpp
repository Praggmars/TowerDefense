#include "pch.h"
#include "level.h"

namespace TowerDefense
{
	namespace content
	{
		GameResources::GameResources(gfx::Graphics& graphics)
		{
			gfx::ModelLoader ml;
			ml.CreateCube(mth::float3(-0.45f), mth::float3(0.9f));
			turretTexture = gfx::Texture::CreateColoredTexture(graphics, { 200, 255, 200, 255 }, 16);
			turretNormalmap = gfx::Texture::CreateDefaultNormalmap(graphics, 16);
			turretModel = gfx::Model::CreateP(graphics, ml);
			turretHitbox = phy::Hitbox::CreateP(ml);

			ml.CreateCube(mth::float3(-0.35f), mth::float3(0.7f));
			enemyTexture = gfx::Texture::CreateColoredTexture(graphics, { 255, 200, 200, 255 }, 16);
			enemyNormalmap = gfx::Texture::CreateDefaultNormalmap(graphics, 16);
			enemyModel = gfx::Model::CreateP(graphics, ml);
			enemyHitbox = phy::Hitbox::CreateP(ml);

			ml.CreateQuad(mth::float2(-0.475f), mth::float2(0.95f), 0.0f);
			areaTexture = gfx::Texture::CreateColoredTexture(graphics, { 200, 200, 255, 255 }, 16);
			areaNormalmap = gfx::Texture::CreateDefaultNormalmap(graphics, 16);
			areaModel = gfx::Model::CreateP(graphics, ml);
			areaHitbox = phy::Hitbox::CreateP(ml);
		}

		Level::Level(gfx::Scene::P scene, GameResources& gameResources, int width, int height) :
			m_width(width), m_height(height)
		{
			scene->BackgroundColor(mth::float4(0.8f, 0.4f, 0.2f, 1.0f));
			m_places.reserve(width * height);

			for (int y = 0; y < m_height; y++)
			{
				for (int x = 0; x < m_width; x++)
				{
					gfx::Material::P material = gfx::Material::CreateP(gameResources.areaTexture, gameResources.areaNormalmap);
					material->MaterialBuffer().color = mth::float4(0.0f, 0.0f, 0.0f, 1.0f);
					PlacementArea::P e = PlacementArea::CreateP(gameResources.areaModel, &material, 1, gameResources.areaHitbox);
					e->position.x = x - m_width * 0.5f;
					e->position.z = y - m_height * 0.5f;
					scene->AddEntity(e);
					m_places.push_back(e);
				}
			}
		}
		PlacementArea::P Level::Place(int x, int y)
		{
			return m_places[y * m_width + x];
		}
		Level::P Level::CreateP(gfx::Scene::P scene, GameResources& gameResources, int width, int height)
		{
			return std::make_shared<Level>(scene, gameResources, width, height);
		}
		Level::U Level::CreateU(gfx::Scene::P scene, GameResources& gameResources, int width, int height)
		{
			return std::make_unique<Level>(scene, gameResources, width, height);
		}
		void Level::Update(float delta)
		{
			for (auto& a : m_places)
				for (auto& m : a->Materials())
					m->MaterialBuffer().textureWeight = 1.0f;
		}
		PlacementArea::P Level::PointedArea(mth::float3 origin, mth::float3 direction)
		{
			PlacementArea::P area;
			float minDistance = INFINITY;
			for (auto& a : m_places)
			{
				if (a->Hitbox())
				{
					float distance = a->Hitbox()->DistanceInDirection(origin, direction, a->WorldMatrix());
					if (distance < minDistance)
					{
						minDistance = distance;
						area = a;
					}
				}
			}
			return area;
		}
		void Level::HighlightPath(mth::vec2<int> start, mth::vec2<int> end)
		{
			class PathFinder
			{
				using Point = mth::vec2<int>;
				struct Node
				{
					enum class Type { BLOCK, PATH, EMPTY, START, END };
					Type type;
					unsigned cost;
					Point prev;
					bool needCheck;
				};

				unsigned m_width, m_height;
				std::vector<Node> nodes;
				Point m_start;
				Point m_end;

			private:
				inline Node& node(int x, int y) { return nodes[y * m_width + x]; }
				inline Node& node(Point p) { return nodes[p.y * m_width + p.x]; }
				bool LeastCostEstimate(Point& best)
				{
					unsigned least = UINT32_MAX;
					for (int y = 0; y < m_height; y++)
					{
						for (int x = 0; x < m_width; x++)
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

				PathFinder(PlacementArea::P* places, unsigned width, unsigned height) :
					m_width(width), m_height(height)
				{
					nodes.resize(width * height);
					for (int i = 0; i < width * height; i++)
						nodes[i].type = places[i]->HasTurret() ? Node::Type::BLOCK : Node::Type::EMPTY;
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
							Point(current.x - 1, current.y),
							Point(current.x + 1, current.y),
							Point(current.x, current.y - 1),
							Point(current.x, current.y + 1) };
						for (Point p : points)
						{
							if (p.x >= 0 && p.x < m_width && p.y >= 0 && p.y < m_height)
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

				void ColorPlaces(PlacementArea::P* places)
				{
					for (int i = 0; i < m_width * m_height; i++)
					{
						if (nodes[i].type == Node::Type::PATH || nodes[i].type == Node::Type::START || nodes[i].type == Node::Type::END)
						{
							for (auto& m : places[i]->Materials())
								m->MaterialBuffer().textureWeight = 0.25f;
						}
					}
				}
			};

			PathFinder pf(m_places.data(), m_width, m_height);
			pf.FindPath(start, end);
			pf.ColorPlaces(m_places.data());
		}
	}
}