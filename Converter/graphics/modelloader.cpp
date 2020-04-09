#include "pch.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "modelloader.h"
#include <wincodec.h>
#include <sstream>

#pragma comment (lib, "assimp/lib/assimp.lib")
#pragma comment (lib, "windowscodecs.lib")

namespace Converter
{
	namespace gfx
	{
		void ModelLoader::StoreFilePath(const wchar_t* path)
		{
			int lastSlashIndex = -1;
			int lastDotIndex = -1;
			for (int i = 0; path[i]; i++)
			{
				switch (path[i])
				{
				case '\\':
				case '/':
					lastSlashIndex = i;
					break;
				case '.':
					lastDotIndex = i;
					break;
				}
			}
			if (lastDotIndex > lastSlashIndex)
			{
				for (int i = lastSlashIndex + 1; i < lastDotIndex; i++)
					m_bareFileName += path[i];
				for (int i = lastDotIndex; path[i]; i++)
					m_extension += path[i];
			}
			else
			{
				for (int i = lastSlashIndex + 1; path[i]; i++)
					m_bareFileName += path[i];
			}
			if (lastSlashIndex >= 0)
			{
				for (int i = 0; i <= lastSlashIndex; i++)
					m_folderName += path[i];
			}
		}

		void ModelLoader::CreateDefaultTextures()
		{
			TextureData data;
			data.filename = L"defaulttexture";
			data.height = 16;
			data.width = 16;
			data.pixels.resize(data.width * data.height);
			for (Color& c : data.pixels)
			{
				c.a = 0xff;
				c.b = 0xff;
				c.g = 0xff;
				c.r = 0xff;
			}
			m_loadedTextures[data.filename] = data;

			data.filename = L"defaultnormal";
			for (Color& c : data.pixels)
			{
				c.a = 0xff;
				c.b = 0xff;
				c.g = 0x7f;
				c.r = 0x7f;
			}
			m_loadedTextures[data.filename] = data;
		}

		void ModelLoader::LoadImages()
		{
			for (MaterialData& m : m_materials)
			{
				auto tex = m_loadedTextures.find(m.texture);
				if (tex == m_loadedTextures.end())
				{
					if (!m.texture.empty())
						m_loadedTextures[m.texture] = LoadTexture(m.texture);
				}
				tex = m_loadedTextures.find(m.normalmap);
				if (tex == m_loadedTextures.end())
				{
					if (!m.normalmap.empty())
						m_loadedTextures[m.normalmap] = LoadTexture(m.normalmap);
				}
			}
		}
		TextureData ModelLoader::LoadTexture(std::wstring& filename)
		{
			TextureData data;
			HRESULT hr;
			Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
			Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
			Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
			Microsoft::WRL::ComPtr<IWICFormatConverter> converter;

			data.filename = filename;

			hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&factory);
			if (FAILED(hr))
				return data;
			hr = factory->CreateDecoderFromFilename((m_folderName + filename).c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
			if (FAILED(hr))
				return data;
			hr = decoder->GetFrame(0, &frame);
			if (FAILED(hr))
				return data;
			hr = factory->CreateFormatConverter(&converter);
			if (FAILED(hr))
				return data;
			hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom);
			if (FAILED(hr))
				return data;
			hr = converter->GetSize(&data.width, &data.height);
			if (FAILED(hr))
				return data;
			data.pixels.resize(data.width * data.height);
			hr = converter->CopyPixels(nullptr, data.width * 4, data.pixels.size() * sizeof(data.pixels[0]), (BYTE*)data.pixels.data());
			if (FAILED(hr))
				return data;

			return data;
		}

		void ModelLoader::LoadBTDM(const wchar_t* filename)
		{
			std::ifstream infile(filename, std::ios::in | std::ios::binary);
			if (!infile.is_open())
				throw Exception::FileOpen(filename);

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
			infile.read(reinterpret_cast<char*>(m_vertices.data()), sizeof(m_vertices[0]) * vertexCount);
			if (!infile.good()) throw Exception::FileRead(filename);

			m_indices.resize(indexCount);
			infile.read(reinterpret_cast<char*>(m_indices.data()), sizeof(m_indices[0]) * indexCount);
			if (!infile.good()) throw Exception::FileRead(filename);

			m_vertexGroups.resize(groupCount);
			infile.read(reinterpret_cast<char*>(m_vertexGroups.data()), sizeof(m_vertexGroups[0]) * groupCount);
			if (!infile.good()) throw Exception::FileRead(filename);

			m_materials.resize(materialCount);
			for (unsigned i = 0; i < materialCount; i++)
			{
				std::wstringstream ss;
				ss << L"Material" << i;
				m_materials[i].name = ss.str();

				infile.read(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
				if (!infile.good()) throw Exception::FileRead(filename);
				m_materials[i].texture.resize(stringSize + 1);
				infile.read(reinterpret_cast<char*>(&m_materials[i].texture[0]), sizeof(wchar_t) * (stringSize + 1));
				if (!infile.good()) throw Exception::FileRead(filename);

				infile.read(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
				if (!infile.good()) throw Exception::FileRead(filename);
				m_materials[i].normalmap.resize(stringSize + 1);
				infile.read(reinterpret_cast<char*>(&m_materials[i].normalmap[0]), sizeof(wchar_t) * (stringSize + 1));
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
				if (s.eof()) return false;
			} while (sch != ech);
			do
			{
				ech = *expected++;
				if (ech == 0) return true;
				s.get(sch);
			} while (!s.eof() && ech == sch);
			return false;
		}
		bool ScanTill(std::wistream& s, wchar_t expected)
		{
			wchar_t ch;
			for (s.get(ch); !s.eof(); s.get(ch))
				if (ch == expected)
					return true;
			return false;
		}
		std::wstring ScanStringAfterWhiteSpacesToLineEnd(std::wifstream& s)
		{
			std::wstring str;
			wchar_t ch;
			for (s.get(ch); ch == L' '; s.get(ch));
			if (ch == L'\n') return str;
			str += ch;
			for (s.get(ch); ch != L'\n'; s.get(ch))
				str += ch;
			return str;
		}
		void ModelLoader::LoadTTDM(const wchar_t* filename)
		{
			std::wifstream infile(filename, std::ios::in | std::ios::binary);
			if (!infile.is_open())
				throw Exception::FileOpen(filename);
			size_t vertexCount;
			size_t indexCount;
			size_t groupCount;
			size_t materialCount;
			size_t boneCount;

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

			if (!ScanStream(infile, L"Bone count:")) throw Exception::FileRead(filename);
			infile >> boneCount;
			if (!ScanTill(infile, L'\n')) throw Exception::FileRead(filename);

			if (!ScanStream(infile, L"Vertices:")) throw Exception::FileRead(filename);
			m_vertices.resize(vertexCount);
			for (Vertex& v : m_vertices)
			{
				infile >> v.position;
				infile >> v.texcoord;
				infile >> v.normal;
				infile >> v.tangent;
				infile >> v.boneWeights;
				infile >> v.boneIndices;
				if (infile.eof()) throw Exception::FileRead(filename);
			}

			if (!ScanStream(infile, L"Indices:")) throw Exception::FileRead(filename);
			m_indices.resize(indexCount);
			for (unsigned& i : m_indices)
			{
				infile >> i;
				if (infile.eof()) throw Exception::FileRead(filename);
			}

			if (!ScanStream(infile, L"Groups:")) throw Exception::FileRead(filename);
			m_vertexGroups.resize(groupCount);
			for (gfx::VertexGroup& g : m_vertexGroups)
			{
				infile >> g.startIndex;
				infile >> g.indexCount;
				infile >> g.materialIndex;
				if (infile.eof()) throw Exception::FileRead(filename);
			}

			if (!ScanStream(infile, L"Materials:")) throw Exception::FileRead(filename);
			m_materials.resize(materialCount);
			for (MaterialData& m : m_materials)
			{
				if (!ScanStream(infile, L"Name:")) throw Exception::FileRead(filename);
				m.name = ScanStringAfterWhiteSpacesToLineEnd(infile);
				if (!ScanStream(infile, L"Texture:")) throw Exception::FileRead(filename);
				m.texture = ScanStringAfterWhiteSpacesToLineEnd(infile);
				if (!ScanStream(infile, L"Normalmap:")) throw Exception::FileRead(filename);
				m.normalmap = ScanStringAfterWhiteSpacesToLineEnd(infile);
				if (!ScanStream(infile, L"Diffuse color:")) throw Exception::FileRead(filename);
				infile >> m.diffuseColor;
				if (!ScanStream(infile, L"Texture weight:")) throw Exception::FileRead(filename);
				infile >> m.textureWeight;
				if (!ScanStream(infile, L"Specular color:")) throw Exception::FileRead(filename);
				infile >> m.specularColor;
				if (!ScanStream(infile, L"Specular power:")) throw Exception::FileRead(filename);
				infile >> m.specularPower;
			}

			if (!ScanStream(infile, L"Bones:")) throw Exception::FileRead(filename);
			m_bones.resize(boneCount);
			for (Bone& b : m_bones)
			{
				if (!ScanStream(infile, L"Name:")) throw Exception::FileRead(filename);
				b.name = ScanStringAfterWhiteSpacesToLineEnd(infile);
				mth::float3 toBoneTranslate, toBoneRotate;
				if (!ScanStream(infile, L"To bone translate:")) throw Exception::FileRead(filename);
				infile >> toBoneTranslate;
				if (!ScanStream(infile, L"To bone rotate:")) throw Exception::FileRead(filename);
				infile >> toBoneRotate;
				b.toBoneSpace = mth::float4x4::RotationTranslation(toBoneRotate, toBoneTranslate);
				if (!ScanStream(infile, L"Transform:")) throw Exception::FileRead(filename);
				infile >> b.offset;
				if (!ScanStream(infile, L"Rotation:")) throw Exception::FileRead(filename);
				infile >> b.rotation;
				b.transform = mth::float4x4::RotationTranslation(b.rotation, b.offset);
				if (!ScanStream(infile, L"Parent:")) throw Exception::FileRead(filename);
				infile >> b.parentIndex;
				if (!ScanStream(infile, L"Children:")) throw Exception::FileRead(filename);
				wchar_t ch;
				int scanNumber;
				bool sameNumber = false;
				for (infile.get(ch); ch != '\n'; infile.get(ch))
				{
					if (!infile.good()) throw Exception::FileRead(filename);
					if (ch >= '0' && ch <= '9')
					{
						if (sameNumber)
						{
							scanNumber *= 10;
							scanNumber += ch - '0';
						}
						else
						{
							sameNumber = true;
							scanNumber = ch - '0';
						}
					}
					if (ch == ',')
					{
						sameNumber = false;
						b.childIndex.push_back(scanNumber);
					}
				}
			}
		}

#pragma region Test model

		class TestModel
		{
			std::vector<Vertex>& m_vertices;
			std::vector<unsigned>& m_indices;
			std::vector<VertexGroup>& m_vertexGroups;
			std::vector<MaterialData>& m_materials;
			std::vector<Bone>& m_bones;

		private:
			void MakeCube(mth::float3 p, mth::float3 s, unsigned bone);
			void AddBones();

		public:
			TestModel(
				std::vector<Vertex>& vertices,
				std::vector<unsigned>& indices,
				std::vector<VertexGroup>& vertexGroups,
				std::vector<MaterialData>& materials,
				std::vector<Bone>& bones);

			void Create();
		};

		void TestModel::MakeCube(mth::float3 p, mth::float3 s, unsigned bone)
		{
			Vertex v;
			v.boneIndices.x = bone;
			v.boneWeights.x = 1.0f;
			unsigned indexOffset = m_vertices.size();

			v.position = mth::float3(p.x, p.y, p.z);
			m_vertices.push_back(v);
			v.position = mth::float3(p.x + s.x, p.y, p.z);
			m_vertices.push_back(v);
			v.position = mth::float3(p.x + s.x, p.y, p.z + s.z);
			m_vertices.push_back(v);
			v.position = mth::float3(p.x, p.y, p.z + s.z);
			m_vertices.push_back(v);
			v.position = mth::float3(p.x, p.y + s.y, p.z);
			m_vertices.push_back(v);
			v.position = mth::float3(p.x + s.x, p.y + s.y, p.z);
			m_vertices.push_back(v);
			v.position = mth::float3(p.x + s.x, p.y + s.y, p.z + s.z);
			m_vertices.push_back(v);
			v.position = mth::float3(p.x, p.y + s.y, p.z + s.z);
			m_vertices.push_back(v);

			unsigned indices[] = {
				0,1,2,2,3,0,
				4,7,6,6,5,4,
				0,4,5,5,1,0,
				1,5,6,6,2,1,
				2,6,7,7,3,2,
				0,3,7,7,4,0
			};
			for (unsigned i = 0; i < 36; i++)
				m_indices.push_back(indices[i] + indexOffset);
		}

		void TestModel::AddBones()
		{
			Bone b;

			b.parentIndex = -1;
			b.name = L"Bone_0";
			b.toBoneSpace = mth::float4x4::Identity();
			b.transform = mth::float4x4::Identity();
			b.ownIndex = 0;
			b.childIndex.push_back(1);
			m_bones.push_back(b);

			b.parentIndex = 0;
			b.name = L"Bone_1";
			b.toBoneSpace = mth::float4x4::Identity();
			b.transform = mth::float4x4::Identity();
			b.ownIndex = 1;
			b.childIndex.push_back(2);
			m_bones.push_back(b);

			b.parentIndex = 1;
			b.name = L"Bone_2";
			b.toBoneSpace = mth::float4x4::Identity();
			b.transform = mth::float4x4::Identity();
			b.ownIndex = 2;
			m_bones.push_back(b);
		}

		TestModel::TestModel(
			std::vector<Vertex>& vertices,
			std::vector<unsigned>& indices,
			std::vector<VertexGroup>& vertexGroups,
			std::vector<MaterialData>& materials,
			std::vector<Bone>& bones) :
			m_vertices(vertices),
			m_indices(indices),
			m_vertexGroups(vertexGroups),
			m_materials(materials),
			m_bones(bones) {}

		void TestModel::Create()
		{
			MakeCube(mth::float3(-0.5f, 0.0f, -0.5f), mth::float3(1.0f, 2.0f, 1.0f), 0);
			MakeCube(mth::float3(-0.5f, 2.0f, -0.5f), mth::float3(2.0f, 1.0f, 1.0f), 1);
			MakeCube(mth::float3(0.5f, 0.0f, -0.5f), mth::float3(1.0f, 2.0f, 1.0f), 2);

			AddBones();

			MaterialData mat;
			mat.name = L"Material";
			mat.diffuseColor = 1.0f;
			mat.textureWeight = 1.0f;
			mat.specularColor = 0.0f;
			mat.specularPower = 1.0f;
			m_materials.push_back(mat);

			VertexGroup vg;
			vg.materialIndex = 0;
			vg.startIndex = 0;
			vg.indexCount = m_indices.size();
			m_vertexGroups.push_back(vg);
		}

		void ModelLoader::LoadTestModel()
		{
			TestModel(m_vertices, m_indices, m_vertexGroups, m_materials, m_bones).Create();
			UVMap();
			Triangularize();
			CalculateNormals();
			CalculateTangents();
		}

#pragma endregion

#pragma region Assimp loader

		class AssimpLoader
		{
			struct MeshedBone
			{
				std::wstring name;
				mth::float4x4 toBoneSpace;
			};

			std::vector<Vertex>& m_vertices;
			std::vector<unsigned>& m_indices;
			std::vector<VertexGroup>& m_vertexGroups;
			std::vector<MaterialData>& m_materials;
			std::vector<Bone>& m_bones;
			std::map<std::wstring, mth::float4x4> m_boneOffsets;

			const aiScene* m_scene;

		private:
			void StoreMaterials();
			void CountVerticesIndices();
			void StoreVerticesIndices();

			aiNode* BaseNode();
			void StoreBoneOffsets();
			int StoreNode(aiNode* node, int parent);
			void StoreNodeChildren(aiNode* node, int parent);
			void StoreBones();

		public:
			AssimpLoader(
				std::vector<Vertex>& vertices,
				std::vector<unsigned>& indices,
				std::vector<VertexGroup>& vertexGroups,
				std::vector<MaterialData>& materials,
				std::vector<Bone>& bones);

			void Load(std::string& filename);
		};

		unsigned CountChildNodes(aiNode* node)
		{
			unsigned childCount = node->mNumChildren;
			for (unsigned c = 0; c < node->mNumChildren; c++)
				childCount += CountChildNodes(node->mChildren[c]);
			return childCount;
		};
		void StoreAiString(std::wstring& dst, aiString& src)
		{
			dst.clear();
			dst.reserve(src.length + 1);
			for (unsigned i = 0; i < src.length; i++)
				dst += static_cast<wchar_t>(src.data[i]);
		}
		std::wstring StoreAiString(aiString& src)
		{
			std::wstring dst;
			StoreAiString(dst, src);
			return dst;
		}
		mth::float4x4 StoreAiMatrix(aiMatrix4x4& mat)
		{
			mth::float4x4 out;
			memcpy(&out, &mat, sizeof(out));
			return out;
		}

		void AssimpLoader::StoreMaterials()
		{
			m_materials.resize(m_scene->mNumMaterials);
			for (unsigned m = 0; m < m_scene->mNumMaterials; m++)
			{
				aiString str;
				aiMaterial& mat = *m_scene->mMaterials[m];

				if (aiReturn_SUCCESS == mat.Get(AI_MATKEY_NAME, str))
				{
					m_materials[m].name.reserve(str.length + 1);
					for (int i = 0; i < str.length; i++)
						m_materials[m].name += str.data[i];
				}

				if (mat.GetTextureCount(aiTextureType_DIFFUSE) > 0)
				{
					mat.GetTexture(aiTextureType_DIFFUSE, 0, &str);
					int lastSlashIndex = -1;
					for (int i = 0; str.data[i]; i++)
						if (str.data[i] == '\\' || str.data[i] == '/')
							lastSlashIndex = i;
					for (int i = lastSlashIndex + 1; str.data[i]; i++)
						m_materials[m].texture += str.data[i];
				}

				if (mat.GetTextureCount(aiTextureType_NORMALS) > 0)
				{
					mat.GetTexture(aiTextureType_NORMALS, 0, &str);
					int lastSlashIndex = -1;
					for (int i = 0; str.data[i]; i++)
						if (str.data[i] == '\\' || str.data[i] == '/')
							lastSlashIndex = i;
					for (int i = lastSlashIndex + 1; str.data[i]; i++)
						m_materials[m].normalmap += str.data[i];
				}

				m_materials[m].diffuseColor = 1.0f;
				m_materials[m].textureWeight = 1.0f;
				m_materials[m].specularColor = 0.0f;
				m_materials[m].specularPower = 1.0f;
			}
		}
		void AssimpLoader::CountVerticesIndices()
		{
			unsigned vertexCount = 0;
			unsigned indexCount = 0;
			m_vertexGroups.resize(m_scene->mNumMeshes);
			for (unsigned m = 0; m < m_scene->mNumMeshes; m++)
			{
				aiMesh* mesh = m_scene->mMeshes[m];
				m_vertexGroups[m].startIndex = indexCount;
				m_vertexGroups[m].materialIndex = mesh->mMaterialIndex;
				vertexCount += m_scene->mMeshes[m]->mNumVertices;
				for (unsigned i = 0; i < m_scene->mMeshes[m]->mNumFaces; i++)
					indexCount += m_scene->mMeshes[m]->mFaces[i].mNumIndices;
				m_vertexGroups[m].indexCount = indexCount - m_vertexGroups[m].startIndex;
			}
			m_vertices.resize(vertexCount);
			m_indices.resize(indexCount);
		}
		void AssimpLoader::StoreVerticesIndices()
		{
			unsigned vertexCount = 0;
			unsigned indexCount = 0;
			for (unsigned m = 0; m < m_scene->mNumMeshes; m++)
			{
				aiMesh* mesh = m_scene->mMeshes[m];

				for (unsigned v = 0; v < mesh->mNumVertices; v++)
				{
					if (mesh->HasPositions())
					{
						m_vertices[vertexCount + v].position.x = mesh->mVertices[v].x;
						m_vertices[vertexCount + v].position.y = mesh->mVertices[v].y;
						m_vertices[vertexCount + v].position.z = mesh->mVertices[v].z;
					}
					if (mesh->HasTextureCoords(0))
					{
						m_vertices[vertexCount + v].texcoord.x = mesh->mTextureCoords[0][v].x;
						m_vertices[vertexCount + v].texcoord.y = 1.0f - mesh->mTextureCoords[0][v].y;
					}
					if (mesh->HasNormals())
					{
						m_vertices[vertexCount + v].normal.x = mesh->mNormals[v].x;
						m_vertices[vertexCount + v].normal.y = mesh->mNormals[v].y;
						m_vertices[vertexCount + v].normal.z = mesh->mNormals[v].z;
					}
					if (mesh->HasTangentsAndBitangents())
					{
						m_vertices[vertexCount + v].tangent.x = mesh->mTangents[v].x;
						m_vertices[vertexCount + v].tangent.y = mesh->mTangents[v].y;
						m_vertices[vertexCount + v].tangent.z = mesh->mTangents[v].z;
					}
				}

				for (unsigned f = 0; f < mesh->mNumFaces; f++)
				{
					for (unsigned i = 2; i < mesh->mFaces[f].mNumIndices; i++)
					{
						m_indices[indexCount++] = mesh->mFaces[f].mIndices[0] + vertexCount;
						m_indices[indexCount++] = mesh->mFaces[f].mIndices[i - 1] + vertexCount;
						m_indices[indexCount++] = mesh->mFaces[f].mIndices[i] + vertexCount;
					}
				}
				vertexCount += mesh->mNumVertices;
			}
		}

		aiNode* FindNode(aiNode* node, const char* name)
		{
			if (std::strcmp(node->mName.C_Str(), name) == 0)
				return node;
			for (unsigned i = 0; i < node->mNumChildren; i++)
				if (aiNode* ret = FindNode(node->mChildren[i], name))
					return ret;
			return nullptr;
		}
		aiNode* AssimpLoader::BaseNode()
		{
			//return m_scene->mRootNode->mChildren[0]->mChildren[0];
			//aiNode* node = FindNode(m_scene->mRootNode, "Armature");
			aiNode* node = m_scene->mRootNode;
			return node ? node->mNumChildren ? node->mChildren[0] : node : m_scene->mRootNode;
		}
		void AssimpLoader::StoreBoneOffsets()
		{
			for (unsigned m = 0; m < m_scene->mNumMeshes; m++)
			{
				aiMesh* mesh = m_scene->mMeshes[m];
				for (unsigned b = 0; b < mesh->mNumBones; b++)
				{
					auto& bone_ai = *mesh->mBones[b];
					m_boneOffsets[StoreAiString(bone_ai.mName)] = StoreAiMatrix(bone_ai.mOffsetMatrix);
				}
			}
		}
		int AssimpLoader::StoreNode(aiNode* node, int parent)
		{
			Bone b;
			StoreAiString(b.name, node->mName);
			b.parentIndex = parent;
			b.transform = parent == -1 ? mth::float4x4::Identity() : StoreAiMatrix(node->mTransformation);
			b.offset.x = b.transform(0, 3);
			b.offset.y = b.transform(1, 3);
			b.offset.z = b.transform(2, 3);
			b.rotation = static_cast<mth::float3x3>(b.transform).ToRotationAngles();
			b.transform = mth::float4x4::RotationTranslation(b.rotation, b.offset);

			auto offset = m_boneOffsets.find(b.name);
			if (offset != m_boneOffsets.end())
			{
				b.toBoneSpace = offset->second;
				//if (b.parentIndex >= 0) b.toBoneSpace = m_bones[b.parentIndex].toBoneSpace * b.toBoneSpace;
			}
			else
			{
				b.toBoneSpace = mth::float4x4::Identity();
			}

			b.ownIndex = m_bones.size();
			m_bones.push_back(b);
			return b.ownIndex;
		}
		void AssimpLoader::StoreNodeChildren(aiNode* node, int parent)
		{
			int index = StoreNode(node, parent);
			for (unsigned c = 0; c < node->mNumChildren; c++)
			{
				if (parent >= 0)
					m_bones[parent].childIndex.push_back(index);
				StoreNodeChildren(node->mChildren[c], index);
			}
		}

		void AssimpLoader::StoreBones()
		{
			aiNode* baseBone = BaseNode();
			//if (baseBone->mNumChildren) baseBone = baseBone->mChildren[0];

			StoreBoneOffsets();
			StoreNodeChildren(baseBone, -1);

			unsigned boneCount = 0;
			unsigned vertexCount = 0;

			for (unsigned m = 0; m < m_scene->mNumMeshes; m++)
			{
				aiMesh* mesh = m_scene->mMeshes[m];

				for (unsigned b = 0; b < mesh->mNumBones; b++)
				{
					auto& bone_ai = *mesh->mBones[b];
					unsigned boneIndex = 0;
					std::wstring boneName = StoreAiString(bone_ai.mName);
					for (Bone& b : m_bones)
					{
						if (b.name == boneName)
						{
							boneIndex = b.ownIndex;
							break;
						}
					}
					for (unsigned w = 0; w < bone_ai.mNumWeights; w++)
					{
						Vertex& vert = m_vertices[vertexCount + bone_ai.mWeights[w].mVertexId];
						unsigned index = 3;
						if (vert.boneWeights.x == 0.0f)
							index = 0;
						else if (vert.boneWeights.y == 0.0f)
							index = 1;
						else if (vert.boneWeights.z == 0.0f)
							index = 2;
						vert.boneIndices(index) = boneIndex;
						if (index < 3)
							vert.boneWeights(index) = bone_ai.mWeights[w].mWeight;
					}
				}
				vertexCount += mesh->mNumVertices;
				boneCount += mesh->mNumBones;
			}

			if (boneCount == 0)
			{
				Bone bone;
				bone.name = L"Bone";
				bone.parentIndex = -1;
				bone.ownIndex = 0;
				bone.toBoneSpace = mth::float4x4::Identity();
				bone.transform = mth::float4x4::Identity();
				m_bones.push_back(bone);
			}
		}
		AssimpLoader::AssimpLoader(
			std::vector<Vertex>& vertices,
			std::vector<unsigned>& indices,
			std::vector<VertexGroup>& vertexGroups,
			std::vector<MaterialData>& materials,
			std::vector<Bone>& bones) :
			m_vertices(vertices),
			m_indices(indices),
			m_vertexGroups(vertexGroups),
			m_materials(materials),
			m_bones(bones),
			m_scene(nullptr) {}
		void AssimpLoader::Load(std::string& filename)
		{
			Assimp::Importer importer;
			m_scene = importer.ReadFile(filename.c_str(),
				aiProcess_CalcTangentSpace |
				//aiProcess_JoinIdenticalVertices |
				aiProcess_RemoveRedundantMaterials |
				aiProcess_Triangulate |
				aiProcess_SortByPType |
				aiProcess_LimitBoneWeights);
			if (m_scene == NULL)
			{
				auto error = importer.GetErrorString();
				throw std::exception(("Importing " + filename + " failed: " + error).c_str());
			}
			StoreMaterials();
			CountVerticesIndices();
			StoreVerticesIndices();
			StoreBones();
		}

		void ModelLoader::LoadAssimp()
		{
			std::string filename;
			for (auto ch : m_folderName)
				filename += (char)ch;
			for (auto ch : m_bareFileName)
				filename += (char)ch;
			for (auto ch : m_extension)
				filename += (char)ch;

			AssimpLoader(m_vertices, m_indices, m_vertexGroups, m_materials, m_bones).Load(filename);
		}

#pragma endregion

		Texture::P ModelLoader::FetchTexture(std::wstring& filename, bool image)
		{
			auto tex = m_loadedTextures.find(filename);
			if (tex != m_loadedTextures.end())
			{
				auto& t = tex->second.texture;
				if (t)
					return t;
			}
			return image ? (m_loadedTextures[L"defaulttexture"].texture) : (m_loadedTextures[L"defaultnormal"].texture);
		}

		ModelLoader::ModelLoader() : m_visible(true) {}
		ModelLoader::ModelLoader(const wchar_t* filename) : m_visible(true)
		{
			LoadModel(filename);
		}
		ModelLoader::ModelLoader(Graphics& graphics, const wchar_t* filename) : m_visible(true)
		{
			LoadModel(graphics, filename);
		}
		void ModelLoader::LoadModel(const wchar_t* filename)
		{
			Clear();
			StoreFilePath(filename);

			CreateDefaultTextures();
			if (m_extension == L".btdm")
				LoadBTDM(filename);
			else if (m_extension == L".ttdm")
				LoadTTDM(filename);
			else if (m_extension == L".test")
				LoadTestModel();
			else
				LoadAssimp();

			LoadImages();
		}
		void ModelLoader::LoadModel(Graphics& graphics, const wchar_t* filename)
		{
			LoadModel(filename);
			CreateResources(graphics);
		}
		void ModelLoader::CreateResources(Graphics& graphics)
		{
			m_model = Model::CreateP(graphics, m_vertices.data(), m_vertices.size(), m_indices.data(), m_indices.size());
			for (auto& t : m_loadedTextures)
			{
				auto& tex = t.second;
				if (!tex.pixels.empty())
					tex.texture = Texture::CreateP(graphics, t.second.pixels.data(), t.second.width, t.second.height);
			}
		}
		void ModelLoader::Clear()
		{
			m_vertices.clear();
			m_indices.clear();
			m_vertexGroups.clear();
			m_materials.clear();
			m_folderName.clear();
			m_bareFileName.clear();
			m_extension.clear();
			m_loadedTextures.clear();
			m_model.reset();
		}
		void ModelLoader::Export(const wchar_t* filename)
		{
			std::ofstream outfile(filename, std::ios::out | std::ios::binary);
			if (outfile.is_open())
			{
				unsigned vertexCount = m_vertices.size();;
				unsigned indexCount = m_indices.size();
				unsigned groupCount = m_vertexGroups.size();
				unsigned materialCount = m_materials.size();
				unsigned stringSize;

				outfile.write(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));

				outfile.write(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));

				outfile.write(reinterpret_cast<char*>(&groupCount), sizeof(groupCount));

				outfile.write(reinterpret_cast<char*>(&materialCount), sizeof(materialCount));

				outfile.write(reinterpret_cast<char*>(m_vertices.data()), sizeof(m_vertices[0]) * vertexCount);

				outfile.write(reinterpret_cast<char*>(m_indices.data()), sizeof(m_indices[0]) * indexCount);

				outfile.write(reinterpret_cast<char*>(m_vertexGroups.data()), sizeof(m_vertexGroups[0]) * groupCount);

				for (unsigned i = 0; i < materialCount; i++)
				{
					stringSize = m_materials[i].texture.length();
					outfile.write(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
					outfile.write(reinterpret_cast<const char*>(m_materials[i].texture.data()), sizeof(wchar_t) * (stringSize + 1));

					stringSize = m_materials[i].normalmap.length();
					outfile.write(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
					outfile.write(reinterpret_cast<const char*>(m_materials[i].normalmap.data()), sizeof(wchar_t) * (stringSize + 1));
				}
			}
		}
		std::vector<unsigned char> ModelLoader::WriteToMemoryBinary()
		{
			size_t size = 0;
			unsigned vertexCount = m_vertices.size();;
			unsigned indexCount = m_indices.size();
			unsigned groupCount = m_vertexGroups.size();
			unsigned materialCount = m_materials.size();
			unsigned stringSize;

			size += sizeof(vertexCount);
			size += sizeof(indexCount);
			size += sizeof(groupCount);
			size += sizeof(materialCount);
			size += sizeof(m_vertices[0]) * vertexCount;
			size += sizeof(m_indices[0]) * indexCount;
			size += sizeof(m_vertexGroups[0]) * groupCount;

			for (unsigned i = 0; i < materialCount; i++)
			{
				stringSize = m_materials[i].texture.length();
				size += sizeof(stringSize);
				size += sizeof(wchar_t) * (stringSize + 1);

				stringSize = m_materials[i].normalmap.length();
				size += sizeof(stringSize);
				size += sizeof(wchar_t) * (stringSize + 1);
			}

			std::vector<unsigned char> out(size);
			size = 0;

			memcpy(&out[size], &vertexCount, sizeof(vertexCount));
			size += sizeof(vertexCount);

			memcpy(&out[size], &indexCount, sizeof(indexCount));
			size += sizeof(indexCount);

			memcpy(&out[size], &groupCount, sizeof(groupCount));
			size += sizeof(groupCount);

			memcpy(&out[size], &materialCount, sizeof(materialCount));
			size += sizeof(materialCount);

			memcpy(&out[size], m_vertices.data(), sizeof(m_vertices[0]) * vertexCount);
			size += sizeof(m_vertices[0]) * vertexCount;

			memcpy(&out[size], m_indices.data(), sizeof(m_indices[0]) * indexCount);
			size += sizeof(m_indices[0]) * indexCount;

			memcpy(&out[size], m_vertexGroups.data(), sizeof(m_vertexGroups[0]) * groupCount);
			size += sizeof(m_vertexGroups[0]) * groupCount;

			for (unsigned i = 0; i < materialCount; i++)
			{
				stringSize = m_materials[i].texture.length();
				memcpy(&out[size], &stringSize, sizeof(stringSize));
				size += sizeof(stringSize);
				memcpy(&out[size], m_materials[i].texture.data(), sizeof(wchar_t) * (stringSize + 1));
				size += sizeof(wchar_t) * (stringSize + 1);

				stringSize = m_materials[i].normalmap.length();
				memcpy(&out[size], &stringSize, sizeof(stringSize));
				size += sizeof(stringSize);
				memcpy(&out[size], m_materials[i].normalmap.data(), sizeof(wchar_t) * (stringSize + 1));
				size += sizeof(wchar_t) * (stringSize + 1);
			}
			return std::move(out);
		}
		std::wstring ModelLoader::WriteToMemoryText()
		{
			std::wstringstream ss;
			ss << L"Vertex count: " << m_vertices.size() << '\n';
			ss << L"Index count: " << m_indices.size() << '\n';
			ss << L"Group count: " << m_vertexGroups.size() << '\n';
			ss << L"Material count: " << m_materials.size() << '\n';
			ss << L"Bone count: " << m_bones.size() << '\n';

			ss << L"\nVertices:\n";
			for (Vertex& v : m_vertices)
			{
				ss << v.position << ' ';
				ss << v.texcoord << ' ';
				ss << v.normal << ' ';
				ss << v.tangent << ' ';
				ss << v.boneWeights << ' ';
				ss << v.boneIndices << '\n';
			}

			ss << L"\nIndices:\n";
			for (size_t i = 0; i < m_indices.size(); i += 3)
			{
				ss << m_indices[i + 0] << ' ';
				ss << m_indices[i + 1] << ' ';
				ss << m_indices[i + 2] << '\n';
			}

			ss << L"\nGroups:\n";
			for (gfx::VertexGroup& g : m_vertexGroups)
			{
				ss << g.startIndex << ' ';
				ss << g.indexCount << ' ';
				ss << g.materialIndex << '\n';
			}

			ss << L"\n\nMaterials:\n";
			for (MaterialData& m : m_materials)
			{
				ss << L"\nName: " << m.name << '\n';
				ss << L"Texture: " << m.texture << '\n';
				ss << L"Normalmap: " << m.normalmap << '\n';
				ss << L"Diffuse color: " << m.diffuseColor << '\n';
				ss << L"Texture weight: " << m.textureWeight << '\n';
				ss << L"Specular color: " << m.specularColor << '\n';
				ss << L"Specular power: " << m.specularPower << '\n';
			}

			ss << L"\n\nBones:\n";
			for (Bone& b : m_bones)
			{
				ss << L"\nName: " << b.name << '\n';
				ss << L"To bone translate: " << mth::float3(b.toBoneSpace(0, 3), b.toBoneSpace(1, 3), b.toBoneSpace(2, 3)) << '\n';
				ss << L"To bone rotate: " << static_cast<mth::float3x3>(b.toBoneSpace).ToRotationAngles() << '\n';
				ss << L"Transform: " << b.offset << '\n';
				ss << L"Rotation: " << b.rotation << '\n';
				ss << L"Parent: " << b.parentIndex << '\n';
				ss << L"Children: ";
				if (!b.childIndex.empty())
				{
					ss << b.childIndex[0];
					for (size_t i = 1; i < b.childIndex.size(); i++)
						ss << ", " << b.childIndex[i];
				}
				ss << '\n';
			}

			return ss.str();
		}
		void ModelLoader::FlipInsideOut()
		{
			for (size_t i = 0; i < m_indices.size(); i += 3)
			{
				unsigned tmp = m_indices[i + 1];
				m_indices[i + 1] = m_indices[i + 2];
				m_indices[i + 2] = tmp;
			}
			for (Vertex& v : m_vertices)
			{
				v.normal *= -1.0f;
				v.tangent *= -1.0f;
			}
		}
		void ModelLoader::Transform(mth::float4x4 transform)
		{
			mth::float3x3 surfaceTransform = mth::float3x3(transform).Transposed().Inverse();
			for (Vertex& v : m_vertices)
			{
				v.position = mth::Transform(transform, v.position);
				v.normal = (surfaceTransform * v.normal).Normalized();
				v.tangent = (surfaceTransform * v.tangent).Normalized();
			}
		}
		void ModelLoader::CalculateNormals()
		{
			for (Vertex& v : m_vertices)
				v.normal = 0.0f;
			for (unsigned i = 0; i < m_indices.size(); i += 3)
			{
				mth::float3 v1(m_vertices[m_indices[i + 0]].position);
				mth::float3 v2(m_vertices[m_indices[i + 1]].position);
				mth::float3 v3(m_vertices[m_indices[i + 2]].position);
				mth::float3 n = (v1 - v2).Cross(v1 - v3);
				m_vertices[m_indices[i + 0]].normal += n;
				m_vertices[m_indices[i + 1]].normal += n;
				m_vertices[m_indices[i + 2]].normal += n;
			}
			for (Vertex& v : m_vertices)
				v.normal.Normalize();
		}
		void ModelLoader::UVMap()
		{
			for (Vertex& v : m_vertices)
			{
				mth::float3 p = v.position.Normalized();
				if (mth::isNear(fabsf(p.y), 1.0f))
				{
					v.texcoord.x = 0.5f;
					v.texcoord.y = p.y < 0.0f ? 1.0f : 0.0f;
				}
				else
				{
					v.texcoord.x = (atan2f(p.x, -p.z) + mth::PI) / (mth::PI * 2.0f);
					v.texcoord.y = acosf(p.y) / mth::PI;
				}
			}
		}
		void ModelLoader::CalculateTangents()
		{
			for (Vertex& v : m_vertices)
				v.tangent = 0.0f;
			for (unsigned i = 0; i < m_indices.size(); i += 3)
			{
				mth::float3 vertex(m_vertices[m_indices[i + 0]].position);
				mth::float2 texcoord(m_vertices[m_indices[i + 0]].texcoord);
				mth::float3 v1 = m_vertices[m_indices[i + 1]].position - vertex;
				mth::float3 v2 = m_vertices[m_indices[i + 2]].position - vertex;
				mth::float2 t1 = m_vertices[m_indices[i + 1]].texcoord - texcoord;
				mth::float2 t2 = m_vertices[m_indices[i + 2]].texcoord - texcoord;
				float den = (t1.x * t2.y - t1.y * t2.x);
				if (den != 0.0f)
					den = 1.0f / den;
				m_vertices[m_indices[i + 0]].tangent += (t1.x * v1 - t1.y * v2) * den;
				m_vertices[m_indices[i + 1]].tangent += (t1.x * v1 - t1.y * v2) * den;
				m_vertices[m_indices[i + 2]].tangent += (t1.x * v1 - t1.y * v2) * den;
			}
			for (Vertex& v : m_vertices)
				v.tangent.Normalize();
		}
		void ModelLoader::Triangularize()
		{
			std::vector<Vertex> vertices(m_indices.size());
			for (size_t i = 0; i < m_indices.size(); i++)
				vertices[i] = m_vertices[m_indices[i]];
			m_vertices = std::move(vertices);
			for (unsigned i = 0; i < m_indices.size(); i++)
				m_indices[i] = i;
		}
		void ModelLoader::RenderMesh(Graphics& graphics)
		{
			if (m_visible && m_model)
			{
				m_model->SetBuffersToRender(graphics);
				m_model->RenderGroup(graphics, 0, m_indices.size());
			}
		}
		void ModelLoader::RenderModel(Graphics& graphics)
		{
			if (m_visible && m_model)
			{
				m_model->SetBuffersToRender(graphics);
				for (unsigned i = 0; i < m_vertexGroups.size(); i++)
				{
					auto& mat = m_materials[m_vertexGroups[i].materialIndex];
					FetchTexture(mat.texture, true)->SetToRender(graphics, 0);
					FetchTexture(mat.normalmap, false)->SetToRender(graphics, 1);
					gfx::CB_MaterialBuffer materialBuffer;
					materialBuffer.diffuseColor = mat.diffuseColor;
					materialBuffer.specularColor = mat.specularColor;
					materialBuffer.specularPower = mat.specularPower;
					materialBuffer.textureWeight = mat.textureWeight;
					graphics.WritePSMaterialBuffer(&materialBuffer);
					m_model->RenderGroup(graphics, m_vertexGroups[i].startIndex, m_vertexGroups[i].indexCount);
				}
			}
		}
		std::wstring ModelLoader::OutputFileName()
		{
			return m_folderName + m_bareFileName + L".tdm";
		}
		std::wstring ModelLoader::InputFileName()
		{
			return m_folderName + m_bareFileName + m_extension;
		}
	}
}