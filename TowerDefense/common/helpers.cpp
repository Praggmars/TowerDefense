#include "pch.h"
#include "helpers.h"
#include <d3dcompiler.h>
#include <fstream>

namespace TowerDefense
{
	namespace hlp
	{
		void ThrowIfFailed(HRESULT hr)
		{
			if (FAILED(hr))
				throw Platform::Exception::CreateException(hr);
		}

		Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::string& shaderCode, const char* entry, const char* target)
		{
			Microsoft::WRL::ComPtr<ID3DBlob> shaderBuffer;
			Microsoft::WRL::ComPtr<ID3DBlob> errorMessage;
			HRESULT hr = D3DCompile(shaderCode.c_str(), shaderCode.length(), nullptr, nullptr, nullptr, entry, target, 0, 0, &shaderBuffer, &errorMessage);
			if (FAILED(hr))
			{
				std::string error = (char*)errorMessage->GetBufferPointer();
				throw Platform::Exception::CreateException(hr);
			}
			return shaderBuffer;
		}

		Exception::Exception() :m_what(L"Unknown error") {}
		Exception::Exception(const wchar_t* what) :m_what(what) {}
		Exception Exception::FileOpen(const wchar_t* filename)
		{
			Exception e(L"Could not load file: ");
			e.m_what += filename;
			return e;
		}
		Exception Exception::FileRead(const wchar_t* filename)
		{
			Exception e(L"Error reading file: ");
			e.m_what += filename;
			return e;
		}
	}
}