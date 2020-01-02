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
		window->PointerPressed += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(this, &TowerDefense::MainPage::OnPointerPressed);
		window->PointerMoved += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(this, &TowerDefense::MainPage::OnPointerMoved);
		window->PointerReleased += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::PointerEventArgs^>(this, &TowerDefense::MainPage::OnPointerReleased);

		m_graphics = gfx::Graphics::CreateU();
		m_graphics->SetSwapChainPanel(m_swapChainPanel);

		m_game = content::Game::CreateU(*m_graphics);
	}

	void TowerDefense::MainPage::OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
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
	void MainPage::OnPointerPressed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args)
	{
		m_game->MouseDown(mth::float2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y));
	}
	void MainPage::OnPointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args)
	{
		m_game->MouseMove(mth::float2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y));
	}
	void MainPage::OnPointerReleased(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args)
	{
		m_game->MouseUp(mth::float2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y));
	}
	void MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
		m_game->PlaceCube();
	}
}