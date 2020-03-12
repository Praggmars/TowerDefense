#include "application.h"

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
	for (int x = 0; x < m_pathFinder.Width(); x++)
	{
		for (int y = 0; y < m_pathFinder.Height(); y++)
		{
			NodeType nt = m_pathFinder.Node(x, y);
			SelectObject(hdc, m_blackPen);
			switch (nt.type)
			{
			case NodeType::EMPTY:
				SelectObject(hdc, nt.isCorner ? m_yellowBrush : m_greenBrush);
				break;
			case NodeType::START:
				SelectObject(hdc, m_redBrush);
				break;
			case NodeType::END:
				SelectObject(hdc, m_blueBrush);
				break;
			case NodeType::CORNER:
				SelectObject(hdc, m_yellowBrush);
				break;
			default:
				SelectObject(hdc, m_grayBrush);
			}
			Rectangle(hdc, x * m_squareSize, y * m_squareSize, (x + 1) * m_squareSize, (y + 1) * m_squareSize);
		}
	}
	PaintConnections(hdc);
	PaintPath(hdc);
}

void Application::PaintConnections(HDC hdc)
{
	SelectObject(hdc, m_blackPen);
	for (NodeType* nt : m_pathFinder.Graph())
	{
		for (auto& n : nt->neighbors)
		{
			MoveToEx(hdc, nt->x * m_squareSize + m_squareSize / 2, nt->y * m_squareSize + m_squareSize / 2, nullptr);
			LineTo(hdc, n.neighbor->x * m_squareSize + m_squareSize / 2, n.neighbor->y * m_squareSize + m_squareSize / 2);
		}
	}
}

void Application::PaintPath(HDC hdc)
{
	auto& path = m_pathFinder.Path();
	if (path.size() > 1)
	{
		SelectObject(hdc, m_redPen);
		MoveToEx(hdc, path[0]->x * m_squareSize + m_squareSize / 2, path[0]->y * m_squareSize + m_squareSize / 2, nullptr);
		for (size_t i = 1; i < path.size(); i++)
			LineTo(hdc, path[i]->x * m_squareSize + m_squareSize / 2, path[i]->y * m_squareSize + m_squareSize / 2);
	}
}

Application::Application() :
	m_window(nullptr),
	m_squareSize(20),
	m_greenBrush(nullptr),
	m_redBrush(nullptr),
	m_blueBrush(nullptr),
	m_grayBrush(nullptr),
	m_yellowBrush(nullptr),
	m_blackPen(nullptr),
	m_redPen(nullptr) {}
Application::~Application()
{
	DeleteObject(m_greenBrush);
	DeleteObject(m_redBrush);
	DeleteObject(m_blueBrush);
	DeleteObject(m_grayBrush);
	DeleteObject(m_yellowBrush);
	DeleteObject(m_blackPen);
	DeleteObject(m_redPen);
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

	if (!m_pathFinder.Init())
		PostQuitMessage(0);

	m_blueBrush = CreateSolidBrush(RGB(50, 50, 200));
	m_greenBrush = CreateSolidBrush(RGB(50, 200, 50));
	m_redBrush = CreateSolidBrush(RGB(200, 50, 50));
	m_grayBrush = CreateSolidBrush(RGB(50, 50, 50));
	m_yellowBrush = CreateSolidBrush(RGB(200, 200, 50));
	m_blackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	m_redPen = CreatePen(PS_SOLID, 3, RGB(235, 20, 20));
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
	case WM_LBUTTONDOWN:
		if (m_pathFinder.ToggleWall(LOWORD(lparam) / m_squareSize, HIWORD(lparam) / m_squareSize))
			InvalidateRect(m_window, NULL, FALSE);
		return 0;
	case WM_RBUTTONDOWN:
		if (m_pathFinder.ReplaceStart(LOWORD(lparam) / m_squareSize, HIWORD(lparam) / m_squareSize))
			InvalidateRect(m_window, NULL, FALSE);
		return 0;
	case WM_PAINT:
		Paint();
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
