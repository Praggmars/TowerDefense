#pragma once

#include "MainPage.g.h"
#include "graphics/modelloader.h"
#include "graphics/camera.h"
#include "graphics/shadowmap.h"
#include "graphics/pointlight.h"
#include "graphics/ambientocclusion.h"

namespace Converter
{
	public ref class MainPage sealed
	{
		gfx::Graphics::U m_graphics;
		gfx::ShadowMap::U m_shadowMap;
		gfx::AmbientOcclusion::U m_ssao;
		gfx::Camera m_camera;
		gfx::PointLight m_light;
		std::vector<std::unique_ptr<gfx::ModelLoader>> m_modelLoaders;
		float m_shadowMapSize;

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
		void MaterialList_DropDownOpened(Platform::Object^ sender, Platform::Object^ e);
		void MaterialList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void ModelList_DropDownOpened(Platform::Object^ sender, Platform::Object^ e);
		void ModelList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		void DeleteButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ExportBinButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void ExportTextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void MaterialDataChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e);
		void ModelShownCheckBox_CheckChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
