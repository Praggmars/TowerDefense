#include "pch.h"
#include "MainPage.xaml.h"

namespace TowerDefense
{
	MainPage::MainPage()
	{
		InitializeComponent();
		Windows::UI::Core::CoreWindow^ window = Windows::UI::Xaml::Window::Current->CoreWindow;

		m_swapChainPanel->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &TowerDefense::MainPage::OnSwapChainPanelSizeChanged);
		window->KeyDown += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::KeyEventArgs^>(this, &TowerDefense::MainPage::OnKeyDown);
		window->KeyUp += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::KeyEventArgs^>(this, &TowerDefense::MainPage::OnKeyUp);
		m_swapChainPanel->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &TowerDefense::MainPage::OnPointerPressed);
		m_swapChainPanel->PointerReleased += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &TowerDefense::MainPage::OnPointerReleased);
		m_swapChainPanel->PointerMoved += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &TowerDefense::MainPage::OnPointerMoved);

		m_graphics = gfx::Graphics::CreateU();
		m_graphics->SetSwapChainPanel(m_swapChainPanel);

		m_game = content::Game::CreateU(*m_graphics);
	}

	void MainPage::OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
	{
		m_game->WindowSizeChanged(mth::float2(e->NewSize.Width, e->NewSize.Height));
		Concurrency::critical_section::scoped_lock lock(m_graphics->GetLock());
		m_graphics->Resize(static_cast<unsigned>(e->NewSize.Width), static_cast<unsigned>(e->NewSize.Height));
	}

	void MainPage::OnKeyDown(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args)
	{
		m_game->KeyDown(args->VirtualKey);
	}
	void MainPage::OnKeyUp(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::KeyEventArgs^ args)
	{
		m_game->KeyUp(args->VirtualKey);
	}
	void MainPage::OnPointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
	{
		auto p = e->GetCurrentPoint(nullptr);
		m_game->MouseDown(mth::float2(p->Position.X, p->Position.Y));
	}
	void TowerDefense::MainPage::OnPointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
	{
		auto p = e->GetCurrentPoint(nullptr);
		m_game->MouseUp(mth::float2(p->Position.X, p->Position.Y));
	}
	void MainPage::OnPointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
	{
		auto p = e->GetCurrentPoint(nullptr);
		m_game->MouseMove(mth::float2(p->Position.X, p->Position.Y));
	}
	void MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
		m_game->PlaceTurret();
	}
}
