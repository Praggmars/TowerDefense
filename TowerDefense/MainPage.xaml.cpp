#include "pch.h"
#include "MainPage.xaml.h"

namespace TowerDefense
{
	MainPage::MainPage()
	{
		InitializeComponent();
		Windows::UI::Core::CoreWindow^ window = Windows::UI::Xaml::Window::Current->CoreWindow;

		m_swapChainPanel->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &TowerDefense::MainPage::OnSwapChainPanelSizeChanged);
		m_swapChainPanel->PointerPressed += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &TowerDefense::MainPage::OnPointerPressed);
		m_swapChainPanel->PointerReleased += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &TowerDefense::MainPage::OnPointerReleased);
		m_swapChainPanel->PointerMoved += ref new Windows::UI::Xaml::Input::PointerEventHandler(this, &TowerDefense::MainPage::OnPointerMoved);

		m_graphics = gfx::Graphics::CreateU();
		m_graphics->SetSwapChainPanel(m_swapChainPanel);
		m_shadowMap = gfx::ShadowMap::CreateU(*m_graphics, 2048);
		m_ambientOcclusion = gfx::AmbientOcclusion::CreateU(*m_graphics);

		m_game = content::Game::CreateU(*m_graphics, *m_shadowMap, *m_ambientOcclusion);
	}

	void MainPage::OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
	{
		m_game->WindowSizeChanged(mth::float2(e->NewSize.Width, e->NewSize.Height));
		Concurrency::critical_section::scoped_lock lock(m_graphics->GetLock());
		m_graphics->Resize(static_cast<unsigned>(e->NewSize.Width), static_cast<unsigned>(e->NewSize.Height));
		m_ambientOcclusion->Resize(*m_graphics);
	}

	void MainPage::OnPointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
	{
		auto p = e->GetCurrentPoint(nullptr);
		m_game->MouseDown(
			mth::float2(p->Position.X, p->Position.Y),
			p->Properties->IsLeftButtonPressed,
			p->Properties->IsMiddleButtonPressed,
			p->Properties->IsRightButtonPressed);
	}
	void TowerDefense::MainPage::OnPointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
	{
		auto p = e->GetCurrentPoint(nullptr);
		m_game->MouseUp(
			mth::float2(p->Position.X, p->Position.Y),
			p->Properties->IsLeftButtonPressed,
			p->Properties->IsMiddleButtonPressed,
			p->Properties->IsRightButtonPressed);
	}
	void MainPage::OnPointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
	{
		auto p = e->GetCurrentPoint(nullptr);
		m_game->MouseMove(
			mth::float2(p->Position.X, p->Position.Y),
			p->Properties->IsLeftButtonPressed,
			p->Properties->IsMiddleButtonPressed,
			p->Properties->IsRightButtonPressed);
	}
	void MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
		m_game->PlaceTurret();
	}
}
