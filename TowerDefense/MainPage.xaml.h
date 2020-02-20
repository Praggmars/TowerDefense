#pragma once

#include "MainPage.g.h"
#include "content/game.h"

namespace TowerDefense
{
	public ref class MainPage sealed
	{
		gfx::Graphics::U m_graphics;
		gfx::ShadowMap::U m_shadowMap;
		gfx::AmbientOcclusion::U m_ambientOcclusion;
		content::Game::U m_game;

	private:
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnPointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void OnPointerReleased(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void OnPointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);

		void OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);

	public:
		MainPage();
	};
}
