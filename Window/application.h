#pragma once

#include <Windows.h>
#include "pathfinder.h"

class Application
{
	HWND m_window;

	PathFinder m_pathFinder;
	int m_squareSize;
	HBRUSH m_greenBrush;
	HBRUSH m_redBrush;
	HBRUSH m_blueBrush;
	HBRUSH m_grayBrush;
	HBRUSH m_yellowBrush;
	HPEN m_blackPen;
	HPEN m_redPen;

private:
	void Paint();
	void Paint(HDC hdc);
	void PaintConnections(HDC hdc);
	void PaintPath(HDC hdc);

public:
	Application();
	~Application();

	void Init(const wchar_t* appName, int width, int height);
	void Run();
	LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};