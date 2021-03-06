#pragma once

#include "pch.h"

#define SMART_PTR(TYPE) using P = std::shared_ptr<TYPE>; using W = std::weak_ptr<TYPE>; using U = std::unique_ptr<TYPE>;
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*a))

namespace TowerDefense
{
	void ThrowIfFailed(HRESULT hr);

	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::string& shaderCode, const char* entry, const char* target);

	class Exception
	{
		std::wstring m_what;

	public:
		Exception();
		Exception(const wchar_t* what);
		static Exception FileOpen(const wchar_t* filename);
		static Exception FileRead(const wchar_t* filename);

		inline const wchar_t* What() { return m_what.c_str(); }
	};
}