#pragma once

#include <Windows.h>
#include "math/geometry2d.hpp"

class Application
{
	HWND m_window;

	POINT m_prevCursor;

	mth::LineSection2Df m_ls1;
	mth::LineSection2Df m_ls2;
	mth::float2* m_movingPoint;
	float m_pointSize;

private:
	void DrawPoint(HDC hdc, mth::float2 p);
	void DrawLineSection(HDC hdc, mth::LineSection2Df& ls);

	void Paint();
	void Paint(HDC hdc);

	void LButtonDownEvent(int x, int y);
	void LButtonUpEvent();
	void MouseMoveEvent(int x, int y);

public:
	Application();
	~Application();

	void Init(const wchar_t* appName, int width, int height);
	void Run();
	LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};