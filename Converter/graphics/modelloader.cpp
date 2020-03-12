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
			infile.read(reinterpret_cast<char*>(m_vertices.data()), sizeof(m_vertices[0])* vertexCount);
			if (!infile.good()) throw Exception::FileRead(filename);

			m_indices.resize(indexCount);
			infile.read(reinterpret_cast<char*>(m_indices.data()), sizeof(m_indices[0])* indexCount);
			if (!infile.good()) throw Exception::FileRead(filename);

			m_vertexGroups.resize(groupCount);
			infile.read(reinterpret_cast<char*>(m_vertexGroups.data()), sizeof(m_vertexGroups[0])* groupCount);
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
				infile.read(reinterpret_cast<char*>(&m_materials[i].texture[0]), sizeof(wchar_t)* (stringSize + 1));
				if (!infile.good()) throw Exception::FileRead(filename);

				infile.read(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
				if (!infile.good()) throw Exception::FileRead(filename);
				m_materials[i].normalmap.resize(stringSize + 1);
				infile.read(reinterpret_cast<char*>(&m_materials[i].normalmap[0]), sizeof(wchar_t)* (stringSize + 1));
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
		}

#pragma region Assimp loader

		void ModelLoader::LoadAssimp()
		{
			std::string filename;
			for (auto ch : m_folderName)
				filename += (char)ch;
			for (auto ch : m_bareFileName)
				filename += (char)ch;
			for (auto ch : m_extension)
				filename += (char)ch;

			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filename.c_str(),
				aiProcess_CalcTangentSpace |
				aiProcess_JoinIdenticalVertices |
				aiProcess_RemoveRedundantMaterials |
				aiProcess_Triangulate |
				aiProcess_SortByPType |
				aiProcess_LimitBoneWeights);
			if (scene == NULL)
			{
				auto error = importer.GetErrorString();
				throw std::exception(("Importing " + filename + " failed: " + error).c_str());
			}

			m_materials.resize(scene->mNumMaterials);
			for (unsigned m = 0; m < scene->mNumMaterials; m++)
			{
				aiString str;
				aiMaterial& mat = *scene->mMaterials[m];

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

			m_vertexGroups.resize(scene->mNumMeshes);
			unsigned vertexCount = 0;
			unsigned indexCount = 0;
			unsigned boneCount = 0;
			for (unsigned m = 0; m < scene->mNumMeshes; m++)
			{
				aiMesh* mesh = scene->mMeshes[m];
				m_vertexGroups[m].startIndex = indexCount;
				m_vertexGroups[m].materialIndex = mesh->mMaterialIndex;
				vertexCount += scene->mMeshes[m]->mNumVertices;
				for (unsigned i = 0; i < scene->mMeshes[m]->mNumFaces; i++)
					indexCount += scene->mMeshes[m]->mFaces[i].mNumIndices;
				m_vertexGroups[m].indexCount = indexCount - m_vertexGroups[m].startIndex;
				boneCount += mesh->mNumBones;
			}
			m_vertices.resize(vertexCount);
			m_indices.resize(indexCount);
			m_bones.resize(boneCount);

			vertexCount = 0;
			indexCount = 0;
			boneCount = 0;
			for (unsigned m = 0; m < scene->mNumMeshes; m++)
			{
				aiMesh* mesh = scene->mMeshes[m];

				for (unsigned v = 0; v < mesh->mNumVertices; v++)
				{
					if (mesh->HasPositions())
					{
						m_vertices[vertexCount + v].position.x = mesh->mVertices[v].x;
						m_vertices[vertexCount + v].position.y = mesh->mVertices[v].y;
						m_vertices[vertexCount + v].position.z = -mesh->mVertices[v].z;
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
						m_vertices[vertexCount + v].normal.z = -mesh->mNormals[v].z;
					}
					if (mesh->HasTangentsAndBitangents())
					{
						m_vertices[vertexCount + v].tangent.x = mesh->mTangents[v].x;
						m_vertices[vertexCount + v].tangent.y = mesh->mTangents[v].y;
						m_vertices[vertexCount + v].tangent.z = -mesh->mTangents[v].z;
					}
				}

				for (unsigned f = 0; f < mesh->mNumFaces; f++)
				{
					for (unsigned i = 2; i < mesh->mFaces[f].mNumIndices; i++)
					{
						m_indices[indexCount++] = mesh->mFaces[f].mIndices[0] + vertexCount;
						m_indices[indexCount++] = mesh->mFaces[f].mIndices[i] + vertexCount;
						m_indices[indexCount++] = mesh->mFaces[f].mIndices[i - 1] + vertexCount;
					}
				}

				//std::vector<std::vector<float>> boneWeights(m_vertices.size());
				for (unsigned b = 0; b < mesh->mNumBones; b++)
				{
					auto& bone = *mesh->mBones[b];
					m_bones[boneCount + b].name.reserve(bone.mName.length + 1);
					for (int i = 0; i < bone.mName.length; i++)
						m_bones[boneCount + b].name += bone.mName.data[i];
					auto matrix = bone.mOffsetMatrix;
					memcpy(&m_bones[boneCount + b].offset, &matrix, sizeof(matrix));
					for (unsigned w = 0; w < bone.mNumWeights; w++)
					{
						Vertex& vert = m_vertices[vertexCount + bone.mWeights[w].mVertexId];
						unsigned index = vert.boneWeights(0);
						m_vertices[vertexCount + bone.mWeights[w].mVertexId];
					}
				}
				vertexCount += mesh->mNumVertices;
				boneCount += mesh->mNumBones;
			}
		}

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

#pragma endregion

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

				outfile.write(reinterpret_cast<char*>(m_vertices.data()), sizeof(m_vertices[0])* vertexCount);

				outfile.write(reinterpret_cast<char*>(m_indices.data()), sizeof(m_indices[0])* indexCount);

				outfile.write(reinterpret_cast<char*>(m_vertexGroups.data()), sizeof(m_vertexGroups[0])* groupCount);

				for (unsigned i = 0; i < materialCount; i++)
				{
					stringSize = m_materials[i].texture.length();
					outfile.write(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
					outfile.write(reinterpret_cast<const char*>(m_materials[i].texture.data()), sizeof(wchar_t)* (stringSize + 1));

					stringSize = m_materials[i].normalmap.length();
					outfile.write(reinterpret_cast<char*>(&stringSize), sizeof(stringSize));
					outfile.write(reinterpret_cast<const char*>(m_materials[i].normalmap.data()), sizeof(wchar_t)* (stringSize + 1));
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
				float den = 1.0f / (t1.x * t2.y - t1.y * t2.x);
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
			m_vertices.swap(vertices);
			for (unsigned i = 0; i < m_indices[i]; i++)
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