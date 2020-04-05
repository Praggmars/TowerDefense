#include "application.h"

int Main()
{
	Application app;
	app.Init(L"Path finding", 20 * 36, 20 * 30);
	app.Run();
	return 0;
}
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR szCmdLine, _In_ INT nCmdShow)
{
	return Main();
}
int main()
{
	return Main();
}