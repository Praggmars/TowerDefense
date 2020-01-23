//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "graphics/modelloader.h"
#include "graphics/camera.h"

namespace Converter
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
		gfx::Graphics::U m_graphics;
		gfx::Camera m_camera;
		gfx::ModelLoader m_modelLoader;

	private:
		void LoadModel(const wchar_t* filename);
		void FillMaterialControls();
		void ReadMaterialControls();

	public:
		MainPage();

		void OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);

		void Draw();
		void OnDragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);
		void OnDrop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e);

	private:
		void OnPointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void OnPointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void OnPointerWheelChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e);
		void m_materialList_DropDownOpened(Platform::Object^ sender, Platform::Object^ e);
		void m_materialList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void ExportBinButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ExportTextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MaterialDataChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
	};
}
