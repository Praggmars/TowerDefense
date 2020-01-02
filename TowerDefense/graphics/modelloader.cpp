#include "pch.h"
#include "modelloader.h"
#include <fstream>

namespace TowerDefense
{
	namespace gfx
	{
		ModelLoader::ModelLoader() {}
		ModelLoader::ModelLoader(const wchar_t* filename, bool binary)
		{
			binary ? LoadBinary(filename) : LoadText(filename);
		}
		void ModelLoader::CreateQuad(mth::float2 position, mth::float2 size, float y)
		{
			Clear();
			m_vertices.resize(4);
			m_vertices[0] = {
				mth::float3(position.x, y, position.y),
				mth::float2(0.0f, 0.0f),
				mth::float3(0.0f, 1.0f, 0.0f),
				mth::float3(0.0f, 0.0f, 1.0f) };
			m_vertices[1] = {
				mth::float3(position.x + size.x, y, position.y),
				mth::float2(0.0f, 1.0f),
				mth::float3(0.0f, 1.0f, 0.0f),
				mth::float3(0.0f, 0.0f, 1.0f) };
			m_vertices[2] = {
				mth::float3(position.x + size.x, y, position.y + size.y),
				mth::float2(1.0f, 1.0f),
				mth::float3(0.0f, 1.0f, 0.0f),
				mth::float3(0.0f, 0.0f, 1.0f) };
			m_vertices[3] = {
				mth::float3(position.x, y, position.y + size.y),
				mth::float2(1.0f, 0.0f),
				mth::float3(0.0f, 1.0f, 0.0f),
				mth::float3(0.0f, 0.0f, 1.0f) };
			m_indices.resize(6);
			m_indices[0] = 0;
			m_indices[1] = 2;
			m_indices[2] = 1;
			m_indices[3] = 3;
			m_indices[4] = 2;
			m_indices[5] = 0;
			m_groups.push_back({ 0, 6, 0 });
			m_materials.push_back({ L"", L"" });
		}
		void ModelLoader::CreateCube(mth::float3 p, mth::float3 s)
		{
			Clear();
			m_vertices.reserve(24);
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y,       p.z + s.z), mth::float2(1.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y,       p.z),       mth::float2(1.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y,       p.z),       mth::float2(0.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y,       p.z + s.z), mth::float2(0.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y + s.y, p.z + s.z), mth::float2(1.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y + s.y, p.z + s.z), mth::float2(0.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y + s.y, p.z),       mth::float2(0.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y + s.y, p.z),       mth::float2(1.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y,       p.z + s.z), mth::float2(1.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y + s.y, p.z + s.z), mth::float2(1.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y + s.y, p.z),       mth::float2(0.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y,       p.z),       mth::float2(0.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y,       p.z),       mth::float2(1.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y + s.y, p.z),       mth::float2(1.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y + s.y, p.z),       mth::float2(0.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y,       p.z),       mth::float2(0.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y,       p.z),       mth::float2(1.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y + s.y, p.z),       mth::float2(1.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y + s.y, p.z + s.z), mth::float2(0.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y,       p.z + s.z), mth::float2(0.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y + s.y, p.z + s.z), mth::float2(0.0f, 0.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x + s.x, p.y,       p.z + s.z), mth::float2(0.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y,       p.z + s.z), mth::float2(1.0f, 1.0f), mth::float3(), mth::float3() });
			m_vertices.push_back({ mth::float3(p.x,       p.y + s.y, p.z + s.z), mth::float2(1.0f, 0.0f), mth::float3(), mth::float3() });
			unsigned indices[] = { 0, 2, 1, 0, 3, 2, 4, 6, 5, 4, 7, 6, 8, 10, 9, 8, 11, 10, 12, 14, 13, 12, 15, 14, 16, 18, 17, 16, 19, 18, 20, 22, 21, 20, 23, 22 };
			m_indices.resize(36);
			memcpy(m_indices.data(), indices, sizeof(indices));
			m_groups.push_back({ 0, 36, 0 });
			m_materials.push_back({ L"", L"" });
			CalculateNormals();
			CalculateTangents();
		}
		void ModelLoader::LoadBinary(const wchar_t* filename)
		{
			Clear();
			std::ifstream infile(filename, std::ios::in | std::ios::binary);
			if (!infile.is_open())
				throw hlp::Exception::FileOpen(filename);
			m_filename = filename;

			unsigned vertexCount;
			unsigned indexCount;
			unsigned groupCount;
			unsigned materialCount;
			unsigned stringSize;

			infile.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			infile.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			infile.read(reinterpret_cast<char*>(&groupCount), sizeof(groupCount));
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			infile.read(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			m_vertices.resize(vertexCount);
			infile.read(reinterpret_cast<char*>(m_vertices.data()), sizeof(m_vertices[0])* vertexCount);
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			m_indices.resize(indexCount);
			infile.read(reinterpret_cast<char*>(m_indices.data()), sizeof(m_indices[0])* indexCount);
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			m_groups.resize(groupCount);
			infile.read(reinterpret_cast<char*>(m_groups.data()), sizeof(m_groups[0])* groupCount);
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			m_materials.resize(materialCount);
			for (unsigned i = 0; i < materialCount; i++)
			{
				infile.read(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
				if (!infile.good()) throw hlp::Exception::FileRead(filename);
				m_materials[i].texture.resize(stringSize + 1);
				infile.read(reinterpret_cast<char*>(m_materials[i].texture.data()), sizeof(wchar_t)* (stringSize + 1));
				if (!infile.good()) throw hlp::Exception::FileRead(filename);

				infile.read(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
				if (!infile.good()) throw hlp::Exception::FileRead(filename);
				m_materials[i].normalmap.resize(stringSize + 1);
				infile.read(reinterpret_cast<char*>(m_materials[i].normalmap.data()), sizeof(wchar_t)* (stringSize + 1));
				if (!infile.good()) throw hlp::Exception::FileRead(filename);
			}
		}
		void ModelLoader::LoadText(const wchar_t* filename)
		{
			Clear();
			std::wifstream infile(filename, std::ios::in | std::ios::binary);
			if (!infile.is_open())
				throw hlp::Exception::FileOpen(filename);
			m_filename = filename;

			unsigned vertexCount;
			unsigned indexCount;
			unsigned groupCount;
			unsigned materialCount;
			unsigned stringSize;

			infile >> vertexCount >> indexCount >> groupCount >> materialCount;
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			m_vertices.resize(vertexCount);
			for (auto& v : m_vertices)
				infile >> v.position >> v.texcoord >> v.normal >> v.tangent;
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			m_indices.resize(indexCount);
			for (auto& i : m_indices)
				infile >> i;
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			m_groups.resize(groupCount);
			for (auto& g : m_groups)
				infile >> g.startIndex >> g.indexCount >> g.materialIndex;
			if (!infile.good()) throw hlp::Exception::FileRead(filename);

			m_materials.resize(materialCount);
			for (unsigned i = 0; i < materialCount; i++)
			{
				infile >> stringSize;
				if (!infile.good()) throw hlp::Exception::FileRead(filename);
				m_materials[i].texture.reserve(stringSize + 1);
				for (size_t i = 0; i < stringSize; i++)
				{
					wchar_t ch;
					infile >> ch;
					m_materials[i].texture += ch;
				}
				if (!infile.good()) throw hlp::Exception::FileRead(filename);

				infile >> stringSize;
				if (!infile.good()) throw hlp::Exception::FileRead(filename);
				m_materials[i].normalmap.reserve(stringSize + 1);
				for (size_t i = 0; i < stringSize; i++)
				{
					wchar_t ch;
					infile >> ch;
					m_materials[i].normalmap += ch;
				}
				if (!infile.good()) throw hlp::Exception::FileRead(filename);
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
	}
}