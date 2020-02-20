#pragma once

#include "texture.h"
#include "model.h"
#include <map>
#include <vector>
#include <string>
#include <fstream>

namespace Converter
{
	namespace gfx
	{
		struct Color
		{
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};
		struct TextureData
		{
			Texture::P texture;
			std::wstring filename;
			std::vector<Color> pixels;
			unsigned width;
			unsigned height;

			TextureData() :width(0), height(0) {}
		};
		struct MaterialData
		{
			std::wstring name;
			std::wstring texture;
			std::wstring normalmap;
			mth::float4 diffuseColor;
			float textureWeight;
			mth::float4 specularColor;
			float specularPower;
		};
		struct VertexGroup
		{
			unsigned startIndex;
			unsigned indexCount;
			unsigned materialIndex;
		};
		struct Bone
		{
			std::wstring name;
			mth::float4x4 offset;
		};

		class ModelLoader
		{
			std::vector<Vertex> m_vertices;
			std::vector<unsigned> m_indices;
			std::vector<VertexGroup> m_vertexGroups;
			std::vector<MaterialData> m_materials;
			std::vector<Bone> m_bones;
			std::wstring m_folderName;
			std::wstring m_bareFileName;
			std::wstring m_extension;
			std::map<std::wstring, TextureData> m_loadedTextures;
			Model::P m_model;
			bool m_visible;

		private:
			void StoreFilePath(const wchar_t* path);
			void CreateDefaultTextures();
			void LoadImages();
			TextureData LoadTexture(std::wstring& filename);

			void LoadTDM(const wchar_t* filename);
			void LoadAssimp();

			Texture::P FetchTexture(std::wstring& filename, bool image);

		public:
			ModelLoader();
			ModelLoader(const wchar_t* filename);
			ModelLoader(Graphics& graphics, const wchar_t* filename);
			void LoadModel(const wchar_t* filename);
			void LoadModel(Graphics& graphics, const wchar_t* filename);
			void CreateResources(Graphics& graphics);
			void Clear();

			void Export(const wchar_t* filename);
			std::vector<unsigned char> WriteToMemory();

			void FlipInsideOut();
			void Transform(mth::float4x4 transform);
			void CalculateNormals();
			void UVMap();
			void CalculateTangents();
			void Triangularize();

			void RenderMesh(Graphics& graphics);
			void RenderModel(Graphics& graphics);

			std::wstring OutputFileName();
			std::wstring InputFileName();
			inline std::wstring& FolderName() { return m_folderName; }
			inline std::wstring& ModelName() { return m_bareFileName; }
			inline Vertex* Vertices() { return m_vertices.data(); }
			inline unsigned VertexCount() { return m_vertices.size(); }
			inline unsigned* Indices() { return m_indices.data(); }
			inline unsigned IndexCount() { return m_indices.size(); }
			inline VertexGroup& VertexGroup(unsigned index) { return m_vertexGroups[index]; }
			inline unsigned VertexGroupCount() { return m_vertexGroups.size(); }
			inline unsigned MaterialCount() { return m_materials.size(); }
			inline MaterialData& Material(unsigned index) { return m_materials[index]; }
			inline Texture::P Image(std::wstring& filename) { return m_loadedTextures[filename].texture; }
			inline void Visible(bool visible) { m_visible = visible; }
			inline bool Visible() { return m_visible; }
		};
	}
}