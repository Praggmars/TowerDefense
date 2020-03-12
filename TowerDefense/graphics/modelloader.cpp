#include "pch.h"
#include "modelloader.h"
#include <fstream>

namespace TowerDefense
{
	namespace gfx
	{
		void ModelLoader::StoreFolderName()
		{
			size_t lastSlash = m_filename.length();
			for (wchar_t ch = m_filename[--lastSlash]; ch != '\\' && ch != '/'; ch = m_filename[--lastSlash])
				if (lastSlash == 0)
					return;
			m_foldername.reserve(lastSlash + 1);
			for (size_t i = 0; i <= lastSlash; i++)
				m_foldername += m_filename[i];
		}
		ModelLoader::ModelLoader() {}
		ModelLoader::ModelLoader(const wchar_t* filename, bool binary)
		{
			binary ? LoadBinary(filename) : LoadText(filename);
		}
		void ModelLoader::CreateQuad(mth::float2 position, mth::float2 size, float y)
		{
			Clear();
			m_vertices.resize(4);
			m_vertices[0] = Vertex{
				mth::float3(position.x, y, position.y),
				mth::float2(0.0f, 0.0f),
				mth::float3(0.0f, 1.0f, 0.0f),
				mth::float3(0.0f, 0.0f, 1.0f),
				mth::float3(),
				mth::vec4<unsigned>() };
			m_vertices[1] = {
				mth::float3(position.x + size.x, y, position.y),
				mth::float2(0.0f, 1.0f),
				mth::float3(0.0f, 1.0f, 0.0f),
				mth::float3(0.0f, 0.0f, 1.0f),
				mth::float3(),
				mth::vec4<unsigned>() };
			m_vertices[2] = {
				mth::float3(position.x + size.x, y, position.y + size.y),
				mth::float2(1.0f, 1.0f),
				mth::float3(0.0f, 1.0f, 0.0f),
				mth::float3(0.0f, 0.0f, 1.0f),
				mth::float3(),
				mth::vec4<unsigned>() };
			m_vertices[3] = {
				mth::float3(position.x, y, position.y + size.y),
				mth::float2(1.0f, 0.0f),
				mth::float3(0.0f, 1.0f, 0.0f),
				mth::float3(0.0f, 0.0f, 1.0f),
				mth::float3(),
				mth::vec4<unsigned>() };
			m_indices.resize(6);
			m_indices[0] = 0;
			m_indices[1] = 2;
			m_indices[2] = 1;
			m_indices[3] = 3;
			m_indices[4] = 2;
			m_indices[5] = 0;
			m_groups.push_back({ 0, 6, 0 });
			m_materials.push_back({ L"quad", L"", L"", { mth::float4(1.0f), mth::float4(), 1.0f, 1.0f } });
		}
		void ModelLoader::CreateCube(mth::float3 p, mth::float3 s)
		{
			Clear();
			m_vertices.reserve(24);
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y,       p.z + s.z), mth::float2(1.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y,       p.z),       mth::float2(1.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y,       p.z),       mth::float2(0.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y,       p.z + s.z), mth::float2(0.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y + s.y, p.z + s.z), mth::float2(1.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y + s.y, p.z + s.z), mth::float2(0.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y + s.y, p.z),       mth::float2(0.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y + s.y, p.z),       mth::float2(1.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y,       p.z + s.z), mth::float2(1.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y + s.y, p.z + s.z), mth::float2(1.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y + s.y, p.z),       mth::float2(0.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y,       p.z),       mth::float2(0.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y,       p.z),       mth::float2(1.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y + s.y, p.z),       mth::float2(1.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y + s.y, p.z),       mth::float2(0.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y,       p.z),       mth::float2(0.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y,       p.z),       mth::float2(1.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y + s.y, p.z),       mth::float2(1.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y + s.y, p.z + s.z), mth::float2(0.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y,       p.z + s.z), mth::float2(0.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y + s.y, p.z + s.z), mth::float2(0.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x + s.x, p.y,       p.z + s.z), mth::float2(0.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y,       p.z + s.z), mth::float2(1.0f, 1.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			m_vertices.push_back(Vertex{ mth::float3(p.x,       p.y + s.y, p.z + s.z), mth::float2(1.0f, 0.0f), mth::float3(), mth::float3(), mth::float3(), mth::vec4<unsigned>() });
			unsigned indices[] = { 0, 2, 1, 0, 3, 2, 4, 6, 5, 4, 7, 6, 8, 10, 9, 8, 11, 10, 12, 14, 13, 12, 15, 14, 16, 18, 17, 16, 19, 18, 20, 22, 21, 20, 23, 22 };
			m_indices.resize(36);
			memcpy(m_indices.data(), indices, sizeof(indices));
			m_groups.push_back({ 0, 36, 0 });
			m_materials.push_back({ L"cube", L"", L"", { mth::float4(1.0f), mth::float4(), 1.0f, 1.0f } });
			CalculateNormals();
			CalculateTangents();
		}
		void ModelLoader::LoadBinary(const wchar_t* filename)
		{
			Clear();
			std::ifstream infile(filename, std::ios::in | std::ios::binary);
			if (!infile.is_open())
				throw Exception::FileOpen(filename);
			m_filename = filename;
			StoreFolderName();

			unsigned vertexCount;
			unsigned indexCount;
			unsigned groupCount;
			unsigned materialCount;
			unsigned stringSize;

			infile.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
			if (!infile.good()) throw Exception::FileRead(filename);

			infile.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
			if (!infile.good()) throw Exception::FileRead(filename);

			infile.read(reinterpret_cast<char*>(&groupCount), sizeof(groupCount));
			if (!infile.good()) throw Exception::FileRead(filename);

			infile.read(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));
			if (!infile.good()) throw Exception::FileRead(filename);

			m_vertices.resize(vertexCount);
			infile.read(reinterpret_cast<char*>(m_vertices.data()), sizeof(m_vertices[0])* vertexCount);
			if (!infile.good()) throw Exception::FileRead(filename);

			m_indices.resize(indexCount);
			infile.read(reinterpret_cast<char*>(m_indices.data()), sizeof(m_indices[0])* indexCount);
			if (!infile.good()) throw Exception::FileRead(filename);

			m_groups.resize(groupCount);
			infile.read(reinterpret_cast<char*>(m_groups.data()), sizeof(m_groups[0])* groupCount);
			if (!infile.good()) throw Exception::FileRead(filename);

			m_materials.resize(materialCount);
			for (unsigned i = 0; i < materialCount; i++)
			{
				infile.read(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
				if (!infile.good()) throw Exception::FileRead(filename);
				m_materials[i].texture.resize(stringSize + 1);
				infile.read(reinterpret_cast<char*>(m_materials[i].texture.data()), sizeof(wchar_t)* (stringSize + 1));
				if (!infile.good()) throw Exception::FileRead(filename);

				infile.read(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
				if (!infile.good()) throw Exception::FileRead(filename);
				m_materials[i].normalmap.resize(stringSize + 1);
				infile.read(reinterpret_cast<char*>(m_materials[i].normalmap.data()), sizeof(wchar_t)* (stringSize + 1));
				if (!infile.good()) throw Exception::FileRead(filename);
			}
		}
		bool ScanStream(std::wistream& s, const wchar_t* expected)
		{
			wchar_t sch, ech; //stream char, expected char
			ech = *expected++;
			do
			{
				s.get(sch);
				if (!s.good()) return false;
			} while (sch != ech);
			do
			{
				ech = *expected++;
				if (ech == 0) return true;
				s.get(sch);
			} while (s.good() && ech == sch);
			return false;
		}
		bool ScanTill(std::wistream& s, wchar_t expected)
		{
			wchar_t ch;
			for (s.get(ch); s.good(); s.get(ch))
				if (ch == expected)
					return true;
			return false;
		}
		std::wstring ScanStringAfterWhiteSpacesToLineEnd(std::wifstream& s)
		{
			std::wstring str;
			wchar_t ch;
			for (s.get(ch); ch == L' '; s.get(ch))
				if (!s.good()) return str;
			if (ch == L'\n') return str;
			str += ch;
			for (s.get(ch); ch != L'\n'; s.get(ch))
			{
				if (!s.good()) return std::wstring();
				str += ch;
			}
			return str;
		}
		void ModelLoader::LoadText(const wchar_t* filename)
		{
			Clear();
			std::wifstream infile(filename, std::ios::in | std::ios::binary);
			if (!infile.is_open())
				throw Exception::FileOpen(filename);
			m_filename = filename;
			StoreFolderName();

			size_t vertexCount;
			size_t indexCount;
			size_t groupCount;
			size_t materialCount;

			if (!ScanStream(infile, L"Vertex count:")) throw Exception::FileRead(filename);
			infile >> vertexCount;
			if (!ScanTill(infile, L'\n')) throw Exception::FileRead(filename);

			if (!ScanStream(infile, L"Index count:")) throw Exception::FileRead(filename);
			infile >> indexCount;
			if (!ScanTill(infile, L'\n')) throw Exception::FileRead(filename);

			if (!ScanStream(infile, L"Group count:")) throw Exception::FileRead(filename);
			infile >> groupCount;
			if (!ScanTill(infile, L'\n')) throw Exception::FileRead(filename);

			if (!ScanStream(infile, L"Material count:")) throw Exception::FileRead(filename);
			infile >> materialCount;
			if (!ScanTill(infile, L'\n')) throw Exception::FileRead(filename);

			if (!ScanStream(infile, L"Vertices:")) throw Exception::FileRead(filename);
			m_vertices.resize(vertexCount);
			vertexCount = 0;
			for (Vertex& v : m_vertices)
			{
				vertexCount++;
				infile >> v.position;
				infile >> v.texcoord;
				infile >> v.normal;
				infile >> v.tangent;
				infile >> v.boneWeights;
				infile >> v.boneIndices;
				if (!infile.good()) throw Exception::FileRead(filename);
			}

			if (!ScanStream(infile, L"Indices:")) throw Exception::FileRead(filename);
			m_indices.resize(indexCount);
			for (unsigned& i : m_indices)
			{
				infile >> i;
				if (!infile.good()) throw Exception::FileRead(filename);
			}

			if (!ScanStream(infile, L"Groups:")) throw Exception::FileRead(filename);
			m_groups.resize(groupCount);
			for (gfx::VertexGroup& g : m_groups)
			{
				infile >> g.startIndex;
				infile >> g.indexCount;
				infile >> g.materialIndex;
				if (!infile.good()) throw Exception::FileRead(filename);
			}

			if (!ScanStream(infile, L"Materials:")) throw Exception::FileRead(filename);
			m_materials.resize(materialCount);
			for (MaterialLoader& m : m_materials)
			{
				if (!ScanStream(infile, L"Name:")) throw Exception::FileRead(filename);
				m.name = ScanStringAfterWhiteSpacesToLineEnd(infile);
				if (!ScanStream(infile, L"Texture:")) throw Exception::FileRead(filename);
				m.texture = ScanStringAfterWhiteSpacesToLineEnd(infile);
				if (!ScanStream(infile, L"Normalmap:")) throw Exception::FileRead(filename);
				m.normalmap = ScanStringAfterWhiteSpacesToLineEnd(infile);
				if (!ScanStream(infile, L"Diffuse color:")) throw Exception::FileRead(filename);
				infile >> m.data.diffuseColor;
				if (!ScanStream(infile, L"Texture weight:")) throw Exception::FileRead(filename);
				infile >> m.data.textureWeight;
				if (!ScanStream(infile, L"Specular color:")) throw Exception::FileRead(filename);
				infile >> m.data.specularColor;
				if (!ScanStream(infile, L"Specular power:")) throw Exception::FileRead(filename);
				infile >> m.data.specularPower;
			}
		}
		void ModelLoader::Clear()
		{
			m_vertices.clear();
			m_indices.clear();
			m_groups.clear();
			m_filename.clear();
		}
		void ModelLoader::CalculateNormals()
		{
			for (Vertex& v : m_vertices)
				v.normal = 0.0f;
			for (size_t i = 0; i < m_indices.size(); i += 3)
			{
				mth::float3 v = m_vertices[m_indices[i + 0]].position;
				mth::float3 n = (v - m_vertices[m_indices[i + 1]].position).Cross(v - m_vertices[m_indices[i + 2]].position);
				m_vertices[m_indices[i + 0]].normal += n;
				m_vertices[m_indices[i + 1]].normal += n;
				m_vertices[m_indices[i + 2]].normal += n;
			}
			for (Vertex& v : m_vertices)
				v.normal.Normalize();
		}
		void ModelLoader::CalculateTangents()
		{
			for (Vertex& v : m_vertices)
				v.tangent = 0.0f;
			for (size_t i = 0; i < m_indices.size(); i += 3)
			{
				mth::float3 vertex = m_vertices[m_indices[i + 0]].position;
				mth::float2 texcoord = m_vertices[m_indices[i + 0]].texcoord;
				mth::float3 v1 = m_vertices[m_indices[i + 1]].position - vertex;
				mth::float3 v2 = m_vertices[m_indices[i + 2]].position - vertex;
				mth::float2 t1 = m_vertices[m_indices[i + 1]].texcoord - texcoord;
				mth::float2 t2 = m_vertices[m_indices[i + 2]].texcoord - texcoord;
				mth::float3 tangent = (t1.x * v1 - t1.y * v2) / (t1.x * t2.y - t1.y * t2.x);
				m_vertices[m_indices[i + 0]].tangent += tangent;
				m_vertices[m_indices[i + 1]].tangent += tangent;
				m_vertices[m_indices[i + 2]].tangent += tangent;
			}
			for (Vertex& v : m_vertices)
				v.tangent.Normalize();
		}
		void ModelLoader::CalculateBoundingBox()
		{
			if (m_vertices.empty()) return;
			m_boundingBox.p1 = m_boundingBox.p2 = m_vertices[0].position;
			for (Vertex& v : m_vertices)
			{
				m_boundingBox.p1.x = min(m_boundingBox.p1.x, v.position.x);
				m_boundingBox.p1.y = min(m_boundingBox.p1.y, v.position.y);
				m_boundingBox.p1.z = min(m_boundingBox.p1.z, v.position.z);
				m_boundingBox.p2.x = max(m_boundingBox.p2.x, v.position.x);
				m_boundingBox.p2.y = max(m_boundingBox.p2.y, v.position.y);
				m_boundingBox.p2.z = max(m_boundingBox.p2.z, v.position.z);
			}
		}
	}
}