#include "MainPage.xaml.h"
//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <ppltasks.h>
using namespace concurrency;
using namespace Windows::Devices::Enumeration;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Converter
{
	MainPage::MainPage()
	{
		InitializeComponent();
		Windows::UI::Core::CoreWindow^ window = Windows::UI::Xaml::Window::Current->CoreWindow;

		m_graphics = gfx::Graphics::CreateU();
		m_graphics->SetSwapChainPanel(m_swapChainPanel);
		m_swapChainPanel->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &Converter::MainPage::OnSizeChanged);
		Draw();
	}
	void MainPage::OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
	{
		Concurrency::critical_section::scoped_lock lock(m_graphics->GetLock());
		m_graphics->Resize(static_cast<unsigned>(e->NewSize.Width), static_cast<unsigned>(e->NewSize.Height));
		m_camera.ScreenAspectRatio(e->NewSize.Width / e->NewSize.Height);
		Draw();
	}
	void MainPage::OnDragOver(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
	{
		e->AcceptedOperation = Windows::ApplicationModel::DataTransfer::DataPackageOperation::Copy;
	}
	void MainPage::OnDrop(Platform::Object^ sender, Windows::UI::Xaml::DragEventArgs^ e)
	{
		auto task = create_task(e->DataView->GetStorageItemsAsync());
		task.then([this](IVectorView<IStorageItem^>^ items) {
			if (auto item = items->First())
			{
				auto path = item->Current->Path;
				std::wstring filename(path->Begin(), path->End());
				LoadModel(filename.c_str());
			}
			});
	}
	void MainPage::OnPointerPressed(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
	{
		auto p = e->GetCurrentPoint(nullptr);
		m_camera.StartTracking(mth::float2(p->Position.X, p->Position.Y));
	}
	void MainPage::OnPointerMoved(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
	{
		auto p = e->GetCurrentPoint(nullptr);
		if (m_camera.Track(mth::float2(p->Position.X, p->Position.Y), p->Properties->IsLeftButtonPressed, p->Properties->IsRightButtonPressed))
			Draw();
	}
	void MainPage::OnPointerWheelChanged(Platform::Object^ sender, Windows::UI::Xaml::Input::PointerRoutedEventArgs^ e)
	{
		auto p = e->GetCurrentPoint(nullptr);
		m_camera.Zoom(p->Properties->MouseWheelDelta > 0 ? 1.1f : 1.0f / 1.1f);
		Draw();
	}

	void MainPage::LoadModel(const wchar_t* filename)
	{
		m_modelLoader.LoadModel(*m_graphics, filename);
		m_filenameTextBox->Text = ref new String(m_modelLoader.OutputFileName().c_str());
		Draw();
	}

	void MainPage::FillMaterialControls()
	{
		int index = m_materialList->SelectedIndex;
		if (index < 0)
		{
			m_textureTextBox->Text = "";
			m_normalmapTextBox->Text = "";
			m_redSlider->Value = 0.0;
			m_greenSlider->Value = 0.0;
			m_blueSlider->Value = 0.0;
			m_alphaSlider->Value = 0.0;
			m_colorWeightSlider->Value = 0.0;
		}
		else
		{
			auto& matData = m_modelLoader.Material(index);
			m_textureTextBox->Text = ref new String(matData.texture.c_str());
			m_normalmapTextBox->Text = ref new String(matData.normalmap.c_str());
			m_redSlider->Value = matData.color.x;
			m_greenSlider->Value = matData.color.y;
			m_blueSlider->Value = matData.color.z;
			m_alphaSlider->Value = matData.color.w;
			m_colorWeightSlider->Value = matData.colorWeight;
		}
	}
	void MainPage::ReadMaterialControls()
	{
		int index = m_materialList->SelectedIndex;
		if (index >= 0)
		{
			auto& matData = m_modelLoader.Material(index);
			matData.color.x = static_cast<float>(m_redSlider->Value);
			matData.color.y = static_cast<float>(m_greenSlider->Value);
			matData.color.z = static_cast<float>(m_blueSlider->Value);
			matData.color.w = static_cast<float>(m_alphaSlider->Value);
			matData.colorWeight = static_cast<float>(m_colorWeightSlider->Value);
			Draw();
		}
	}

	void MainPage::Draw()
	{
		mth::float4x4 matrixBuffer[] = {
			mth::float4x4::Identity(),
			m_camera.CameraMatrix()
		};
		float lightBuffer[] = {
			1.0f, 1.0f, 1.0f, 1.0f,
			m_camera.position.x, m_camera.position.y, m_camera.position.z,
			0.7f
		};
		m_graphics->WriteVSMatrixBuffer(matrixBuffer);
		m_graphics->WritePSLightBuffer(lightBuffer);

		m_graphics->Clear(0.2f, 0.3f, 0.5f);
		m_modelLoader.RenderModel(*m_graphics);
		m_graphics->Present();
	}

	void MainPage::m_materialList_DropDownOpened(Platform::Object^ sender, Platform::Object^ e)
	{
		m_materialList->Items->Clear();
		for (unsigned i = 0; i < m_modelLoader.MaterialCount(); i++)
		{
			auto& m = m_modelLoader.Material(i);
			m_materialList->Items->Append(ref new String(m.name.c_str(), m.name.length()));
		}
	}
	void MainPage::m_materialList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
	{
		if (m_materialList->SelectedIndex < 0)
		{
			m_textureTextBox->Text = "";
			m_normalmapTextBox->Text = "";
		}
		FillMaterialControls();
	}
	void MainPage::ExportBinButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
		FileSavePicker^ filePicker = ref new FileSavePicker;
		filePicker->SuggestedFileName = ref new String(m_modelLoader.OutputFileName().c_str());
		//filePicker->FileTypeChoices->Insert()
		auto task = create_task(filePicker->PickSaveFileAsync());
		task.then([this](Windows::Storage::StorageFile^ file) {
			
			m_modelLoader.Export(file->Path->Data());
			});
	}
	void MainPage::ExportTextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
	}
	void MainPage::MaterialDataChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
	{
		ReadMaterialControls();
	}
}
