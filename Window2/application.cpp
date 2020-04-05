#include "application.h"

void Application::DrawPoint(HDC hdc, mth::float2 p)
{
	Ellipse(hdc,
		static_cast<int>(p.x - m_pointSize),
		static_cast<int>(p.y - m_pointSize),
		static_cast<int>(p.x + m_pointSize),
		static_cast<int>(p.y + m_pointSize));
}

void Application::DrawLineSection(HDC hdc, mth::LineSection2Df& ls)
{
	DrawPoint(hdc, ls.point1);
	DrawPoint(hdc, ls.point2);
	MoveToEx(hdc, static_cast<int>(ls.point1.x), static_cast<int>(ls.point1.y), nullptr);
	LineTo(hdc, static_cast<int>(ls.point2.x), static_cast<int>(ls.point2.y));
}

void Application::Paint()
{
	PAINTSTRUCT ps;
	RECT rect;
	GetClientRect(m_window, &rect);
	HDC hdc = BeginPaint(m_window, &ps);
	HDC hmem = CreateCompatibleDC(hdc);
	HBITMAP hbmp = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
	SelectObject(hmem, hbmp);

	PatBlt(hmem, rect.left, rect.top, rect.right, rect.bottom, WHITENESS);
	Paint(hmem);

	BitBlt(hdc, rect.left, rect.top, rect.right, rect.bottom, hmem, 0, 0, SRCCOPY);
	DeleteObject(hmem);
	DeleteDC(hmem);

	EndPaint(m_window, &ps);
}

void Application::Paint(HDC hdc)
{
	DrawLineSection(hdc, m_ls1);
	DrawLineSection(hdc, m_ls2);

	mth::float2 intersection;
	if (m_ls1.Intersects(m_ls2, &intersection))
		DrawPoint(hdc, intersection);
}

void Application::LButtonDownEvent(int x, int y)
{
	mth::float2 c(static_cast<float>(x), static_cast<float>(y));
	float distSqr = m_pointSize * m_pointSize;

	if ((m_ls1.point1 - c).LengthSquare() < distSqr)
		m_movingPoint = &m_ls1.point1;
	else if ((m_ls1.point2 - c).LengthSquare() < distSqr)
		m_movingPoint = &m_ls1.point2;
	else if ((m_ls2.point1 - c).LengthSquare() < distSqr)
		m_movingPoint = &m_ls2.point1;
	else if ((m_ls2.point2 - c).LengthSquare() < distSqr)
		m_movingPoint = &m_ls2.point2;
}

void Application::LButtonUpEvent()
{
	m_movingPoint = nullptr;
}

void Application::MouseMoveEvent(int x, int y)
{
	mth::float2 c(static_cast<float>(x), static_cast<float>(y));
	int dx = x - m_prevCursor.x;
	int dy = y - m_prevCursor.y;
	m_prevCursor.x = x;
	m_prevCursor.y = y;
	if (m_movingPoint)
	{
		*m_movingPoint = c;
		InvalidateRect(m_window, nullptr, false);
	}
}

Application::Application() :
	m_window(nullptr),
	m_prevCursor{ 0, 0 },
	m_ls1(mth::float2(100.0f, 100.0f), mth::float2(200.0f, 100.0f)),
	m_ls2(mth::float2(100.0f, 200.0f), mth::float2(200.0f, 200.0f)),
	m_movingPoint(nullptr),
	m_pointSize(5.0f) {}
Application::~Application()
{
}
void Application::Init(const wchar_t* appName, int width, int height)
{
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = appName;
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
		if (msg == WM_CREATE)
		{
			LRESULT(*WndProc)(HWND, UINT, WPARAM, LPARAM) = [](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
				return reinterpret_cast<Application*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))->MessageHandler(hwnd, msg, wparam, lparam);
			};
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(lparam)->lpCreateParams));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc));
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);
	};
	RegisterClassEx(&wc);

	RECT rect;
	rect.left = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	rect.top = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	DWORD exStyle = WS_EX_OVERLAPPEDWINDOW;
	DWORD style = WS_OVERLAPPEDWINDOW;
	AdjustWindowRectEx(&rect, style, false, exStyle);

	m_window = CreateWindowEx(exStyle, appName, appName, style, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, wc.hInstance, this);

	ShowWindow(m_window, SW_SHOWDEFAULT);
}

void Application::Run()
{
	MSG msg{};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT Application::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		if (wparam == VK_SPACE)
			InvalidateRect(m_window, nullptr, false);
	case WM_LBUTTONDOWN:
		LButtonDownEvent(LOWORD(lparam), HIWORD(lparam));
		return 0;
	case WM_LBUTTONUP:
		LButtonUpEvent();
		return 0;
	case WM_MOUSEMOVE:
		MouseMoveEvent(LOWORD(lparam), HIWORD(lparam));
		return 0;
	case WM_PAINT:
		Paint();
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
