#include "MainPage.xaml.h"

#include "pch.h"
#include "MainPage.xaml.h"
#include <ppltasks.h>
#include <sstream>
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

namespace Converter
{
	MainPage::MainPage() :
		m_shadowMapSize(2048.0f)
	{
		InitializeComponent();
		Windows::UI::Core::CoreWindow^ window = Windows::UI::Xaml::Window::Current->CoreWindow;

		m_light.position = mth::float3(4.0f, 4.5f, -5.5f);
		m_light.rotation = mth::float3(0.33f, -0.62f, 0.0f);
		m_graphics = gfx::Graphics::CreateU();
		m_graphics->SetSwapChainPanel(m_swapChainPanel);
		m_shadowMap = gfx::ShadowMap::CreateU(*m_graphics, static_cast<unsigned>(m_shadowMapSize), static_cast<unsigned>(m_shadowMapSize));
		m_ssao = gfx::AmbientOcclusion::CreateU(*m_graphics);
		m_swapChainPanel->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &Converter::MainPage::OnSizeChanged);

		//LoadModel(LR"(F:\Study\TowerDefense\Bin\x64\Debug\Converter\AppX\res\plain.obj)");
		//LoadModel(LR"(F:\Study\TowerDefense\Bin\x64\Debug\Converter\AppX\res\monkey.obj)");
		//m_modelLoaders[1]->Transform(mth::float4x4::Translation(mth::float3(0.0f, 2.0f, 0.0f)));
		//m_modelLoaders[1]->CreateResources(*m_graphics);

		Draw();
	}
	void MainPage::OnSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e)
	{
		Concurrency::critical_section::scoped_lock lock(m_graphics->GetLock());
		m_graphics->Resize(static_cast<unsigned>(e->NewSize.Width), static_cast<unsigned>(e->NewSize.Height));
		m_camera.ScreenAspectRatio(e->NewSize.Width / e->NewSize.Height);
		m_ssao->Resize(*m_graphics);
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
		try
		{
			std::unique_ptr<gfx::ModelLoader> ml = std::make_unique<gfx::ModelLoader>();
			ml->LoadModel(*m_graphics, filename);
			m_modelList->Items->Append(ref new String(ml->ModelName().c_str()));
			m_modelLoaders.push_back(std::move(ml));
			m_modelList->SelectedIndex = m_modelLoaders.size() - 1;
		}
		catch (std::exception& ex)
		{
			std::string error = ex.what();
			std::wstring werror(error.begin(), error.end());
			Windows::UI::Popups::MessageDialog(ref new String(werror.c_str(), werror.length()));
		}
		catch (Exception& ex)
		{
			Windows::UI::Popups::MessageDialog(ref new String(ex.What()));
		}
	}

	void MainPage::FillMaterialControls()
	{
		int modelIndex = m_modelList->SelectedIndex;
		int materialIndex = m_materialList->SelectedIndex;
		if (modelIndex < 0 || materialIndex < 0)
		{
			m_textureTextBox->Text = "";
			m_normalmapTextBox->Text = "";
			m_diffuseRedSlider->Value = 0.0;
			m_diffuseGreenSlider->Value = 0.0;
			m_diffuseBlueSlider->Value = 0.0;
			m_diffuseAlphaSlider->Value = 0.0;
			m_textureWeightSlider->Value = 0.0;
			m_specularRedSlider->Value = 0.0;
			m_specularGreenSlider->Value = 0.0;
			m_specularBlueSlider->Value = 0.0;
			m_specularAlphaSlider->Value = 0.0;
			m_specularPowerSlider->Value = 0.0;
		}
		else
		{
			auto& matData = m_modelLoaders[modelIndex]->Material(materialIndex);
			m_textureTextBox->Text = ref new String(matData.texture.c_str(), matData.texture.length());
			m_normalmapTextBox->Text = ref new String(matData.normalmap.c_str(), matData.normalmap.length());
			double dr = matData.diffuseColor.x;
			double dg = matData.diffuseColor.y;
			double db = matData.diffuseColor.z;
			double da = matData.diffuseColor.w;
			double tw = matData.textureWeight;
			double sr = matData.specularColor.x;
			double sg = matData.specularColor.y;
			double sb = matData.specularColor.z;
			double sa = matData.specularColor.w;
			double sp = matData.specularPower;
			m_diffuseRedSlider->Value = dr;
			m_diffuseGreenSlider->Value = dg;
			m_diffuseBlueSlider->Value = db;
			m_diffuseAlphaSlider->Value = da;
			m_textureWeightSlider->Value = tw;
			m_specularRedSlider->Value = sr;
			m_specularGreenSlider->Value = sg;
			m_specularBlueSlider->Value = sb;
			m_specularAlphaSlider->Value = sa;
			m_specularPowerSlider->Value = sp;
		}
	}
	void MainPage::ReadMaterialControls()
	{
		int modelIndex = m_modelList->SelectedIndex;
		int materialIndex = m_materialList->SelectedIndex;
		if (modelIndex >= 0 && materialIndex >= 0)
		{
			auto& matData = m_modelLoaders[modelIndex]->Material(materialIndex);
			matData.diffuseColor.x = static_cast<float>(m_diffuseRedSlider->Value);
			matData.diffuseColor.y = static_cast<float>(m_diffuseGreenSlider->Value);
			matData.diffuseColor.z = static_cast<float>(m_diffuseBlueSlider->Value);
			matData.diffuseColor.w = static_cast<float>(m_diffuseAlphaSlider->Value);
			matData.textureWeight = static_cast<float>(m_textureWeightSlider->Value);
			matData.specularColor.x = static_cast<float>(m_specularRedSlider->Value);
			matData.specularColor.y = static_cast<float>(m_specularGreenSlider->Value);
			matData.specularColor.z = static_cast<float>(m_specularBlueSlider->Value);
			matData.specularColor.w = static_cast<float>(m_specularAlphaSlider->Value);
			matData.specularPower = static_cast<float>(m_specularPowerSlider->Value);
			Draw();
		}
	}

	void MainPage::FillBoneControls()
	{
		int modelIndex = m_modelList->SelectedIndex;
		int boneIndex = m_boneList->SelectedIndex;
		if (modelIndex < 0 || boneIndex < 0)
		{
			m_toBoneTxTextBox->Text = "";
			m_toBoneTyTextBox->Text = "";
			m_toBoneTzTextBox->Text = "";
			m_toBoneRxSlider->Value = 0.0;
			m_toBoneRySlider->Value = 0.0;
			m_toBoneRzSlider->Value = 0.0;

			m_transformTxTextBox->Text = "";
			m_transformTyTextBox->Text = "";
			m_transformTzTextBox->Text = "";
			m_transformRxSlider->Value = 0.0;
			m_transformRySlider->Value = 0.0;
			m_transformRzSlider->Value = 0.0;
		}
		else
		{
			auto& boneData = m_modelLoaders[modelIndex]->Bones(boneIndex);
			mth::float3 toBoneT = mth::float3(boneData.toBoneSpace(0, 3), boneData.toBoneSpace(1, 3), boneData.toBoneSpace(2, 3));
			mth::float3 toBoneR = static_cast<mth::float3x3>(boneData.toBoneSpace).ToRotationAngles();
			String^ toBoneTx = ref new String(std::to_wstring(toBoneT.x).c_str());
			String^ toBoneTy = ref new String(std::to_wstring(toBoneT.y).c_str());
			String^ toBoneTz = ref new String(std::to_wstring(toBoneT.z).c_str());
			double toBoneRx = toBoneR.x;
			double toBoneRy = toBoneR.y;
			double toBoneRz = toBoneR.z;

			String^ transformTx = ref new String(std::to_wstring(boneData.offset.x).c_str());
			String^ transformTy = ref new String(std::to_wstring(boneData.offset.y).c_str());
			String^ transformTz = ref new String(std::to_wstring(boneData.offset.z).c_str());
			double transformRx = boneData.rotation.x;
			double transformRy = boneData.rotation.y;
			double transformRz = boneData.rotation.z;

			m_toBoneTxTextBox->Text = toBoneTx;
			m_toBoneTyTextBox->Text = toBoneTy;
			m_toBoneTzTextBox->Text = toBoneTz;
			m_toBoneRxSlider->Value = toBoneRx;
			m_toBoneRySlider->Value = toBoneRy;
			m_toBoneRzSlider->Value = toBoneRz;

			m_transformTxTextBox->Text = transformTx;
			m_transformTyTextBox->Text = transformTy;
			m_transformTzTextBox->Text = transformTz;
			m_transformRxSlider->Value = transformRx;
			m_transformRySlider->Value = transformRy;
			m_transformRzSlider->Value = transformRz;
		}
	}

	void MainPage::ReadBoneControls()
	{
		int modelIndex = m_modelList->SelectedIndex;
		int boneIndex = m_boneList->SelectedIndex;
		if (modelIndex >= 0 && boneIndex >= 0)
		{
			auto& boneData = m_modelLoaders[modelIndex]->Bones(boneIndex);
			{
				mth::float3 toBoneT;
				mth::float3 toBoneR;
				std::wstringstream x(m_toBoneTxTextBox->Text->Data());
				std::wstringstream y(m_toBoneTyTextBox->Text->Data());
				std::wstringstream z(m_toBoneTzTextBox->Text->Data());
				x >> toBoneT.x;
				y >> toBoneT.y;
				z >> toBoneT.z;
				toBoneR.x = static_cast<float>(m_toBoneRxSlider->Value);
				toBoneR.y = static_cast<float>(m_toBoneRySlider->Value);
				toBoneR.z = static_cast<float>(m_toBoneRzSlider->Value);
				boneData.toBoneSpace = mth::float4x4::RotationTranslation(toBoneR, toBoneT);
			}
			{
				std::wstringstream x(m_transformTxTextBox->Text->Data());
				std::wstringstream y(m_transformTyTextBox->Text->Data());
				std::wstringstream z(m_transformTzTextBox->Text->Data());
				x >> boneData.offset.x;
				y >> boneData.offset.y;
				z >> boneData.offset.z;
				boneData.rotation.x = static_cast<float>(m_transformRxSlider->Value);
				boneData.rotation.y = static_cast<float>(m_transformRySlider->Value);
				boneData.rotation.z = static_cast<float>(m_transformRzSlider->Value);
				boneData.transform = mth::float4x4::RotationTranslation(boneData.rotation, boneData.offset);
			}
			Draw();
		}
	}

	void MainPage::Draw()
	{
		gfx::CB_MatrixBuffer matrixBuffer;
		matrixBuffer.worldMatrix = mth::float4x4::Identity();
		matrixBuffer.cameraMatrix = m_camera.CameraMatrix();
		matrixBuffer.lightMatrix = m_light.LightMatrix();
		m_graphics->WriteVSMatrixBuffer(&matrixBuffer);

		gfx::CB_LightBuffer lightBuffer;
		lightBuffer.ambientColor = 0.3f;
		lightBuffer.diffuseColor = 0.7f;
		lightBuffer.sourcePosition = m_light.position;
		lightBuffer.shadowMapDelta = 1.0f / m_shadowMapSize;
		lightBuffer.eyePosition = m_camera.position;
		lightBuffer.screenSize = mth::float2(static_cast<float>(m_graphics->Width()), static_cast<float>(m_graphics->Height()));

		auto WriteBoneBuffer = [this](gfx::ModelLoader& ml)
		{
			ml.Bones(0).boneMatrix = ml.Bones(0).transform;
			for (unsigned i = 1; i < ml.BoneCount(); i++)
			{
				int parentIndex = ml.Bones(i).parentIndex;
				ml.Bones(i).boneMatrix = parentIndex >= 0 ? ml.Bones(parentIndex).boneMatrix * ml.Bones(i).transform : ml.Bones(i).transform;
			}
			std::vector<mth::float4x4> bones(ml.BoneCount());
			for (unsigned i = 0; i < ml.BoneCount(); i++)
				bones[i] = ml.Bones(i).boneMatrix * ml.Bones(i).toBoneSpace;
			m_graphics->WriteBoneBuffer(bones.data(), bones.size());
		};

		m_shadowMap->SetAsRenderTarget(*m_graphics);
		for (auto& ml : m_modelLoaders)
		{
			WriteBoneBuffer(*ml);
			ml->RenderMesh(*m_graphics);
		}

		m_ssao->SetAsRenderTarget(*m_graphics, m_camera);
		for (auto& ml : m_modelLoaders)
		{
			WriteBoneBuffer(*ml);
			ml->RenderMesh(*m_graphics);
		}
		m_ssao->RenderOcclusionMap(*m_graphics, m_camera);

		m_graphics->SetScreenAsRenderTarget();

		m_graphics->WritePSLightBuffer(&lightBuffer);
		m_shadowMap->SetTextureToRender(*m_graphics, 2);
		m_ssao->SetTextureToRender(*m_graphics, 3);

		m_graphics->Clear(0.2f, 0.3f, 0.5f);
		for (auto& ml : m_modelLoaders)
		{
			WriteBoneBuffer(*ml);
			ml->RenderModel(*m_graphics);
		}
		m_graphics->Present();
	}

	void MainPage::MaterialList_DropDownOpened(Platform::Object^ sender, Platform::Object^ e)
	{
		m_materialList->Items->Clear();
		int modelIndex = m_modelList->SelectedIndex;
		if (modelIndex < 0) return;
		gfx::ModelLoader& ml = *m_modelLoaders[modelIndex];
		for (unsigned i = 0; i < ml.MaterialCount(); i++)
		{
			auto& m = ml.Material(i);
			m_materialList->Items->Append(ref new String(m.name.c_str(), m.name.length()));
		}
	}
	void MainPage::MaterialList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
	{
		if (m_materialList->SelectedIndex < 0)
		{
			m_textureTextBox->Text = "";
			m_normalmapTextBox->Text = "";
		}
		FillMaterialControls();
	}
	void MainPage::ModelList_DropDownOpened(Platform::Object^ sender, Platform::Object^ e)
	{
		m_modelList->Items->Clear();
		for (auto& m : m_modelLoaders)
			m_modelList->Items->Append(ref new String(m->ModelName().c_str(), m->ModelName().length()));
	}
	void MainPage::ModelList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
	{
		int modelIndex = m_modelList->SelectedIndex;
		m_materialList->SelectedIndex = -1;
		if (modelIndex < 0)
		{
			m_modelShownCheckBox->IsChecked = false;
		}
		else
		{
			m_modelShownCheckBox->IsChecked = m_modelLoaders[modelIndex]->Visible();
		}
	}
	void MainPage::ModelShownCheckBox_CheckChanged(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
		int modelIndex = m_modelList->SelectedIndex;
		if (modelIndex >= 0)
		{
			m_modelLoaders[modelIndex]->Visible(m_modelShownCheckBox->IsChecked->Value);
			Draw();
		}
	}
	void MainPage::DeleteButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
		int modelIndex = m_modelList->SelectedIndex;
		if (modelIndex >= 0)
		{
			m_modelList->Items->RemoveAt(modelIndex);
			m_modelLoaders.erase(m_modelLoaders.begin() + modelIndex);
			Draw();
		}
	}
	void MainPage::ExportBinButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
		int modelIndex = m_modelList->SelectedIndex;
		if (modelIndex >= 0)
		{
			gfx::ModelLoader& ml = *m_modelLoaders[modelIndex];
			FileSavePicker^ filePicker = ref new FileSavePicker;
			filePicker->SuggestedFileName = ref new String(ml.ModelName().c_str());
			auto types = ref new Platform::Collections::Vector<String^>();
			types->Append(".btdm");
			filePicker->FileTypeChoices->Insert("Binary Tower Defense Model", types);
			auto task = create_task(filePicker->PickSaveFileAsync());
			task.then([&ml](Windows::Storage::StorageFile^ file) {
				if (file)
				{
					std::vector<unsigned char> data = std::move(ml.WriteToMemoryBinary());
					Array<unsigned char>^ buffer = ref new Array<unsigned char>(data.data(), data.size());
					FileIO::WriteBytesAsync(file, buffer);
				}
				});
		}
	}
	void MainPage::ExportTextButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
	{
		int modelIndex = m_modelList->SelectedIndex;
		if (modelIndex >= 0)
		{
			gfx::ModelLoader& ml = *m_modelLoaders[modelIndex];
			FileSavePicker^ filePicker = ref new FileSavePicker;
			filePicker->SuggestedFileName = ref new String(ml.ModelName().c_str());
			auto types = ref new Platform::Collections::Vector<String^>();
			types->Append(".ttdm");
			filePicker->FileTypeChoices->Insert("Text Tower Defense Model", types);
			auto task = create_task(filePicker->PickSaveFileAsync());
			task.then([&ml](Windows::Storage::StorageFile^ file) {
				if (file)
				{
					String^ data = ref new String(ml.WriteToMemoryText().c_str());
					FileIO::WriteTextAsync(file, data);
				}
				});
		}
	}
	void MainPage::MaterialDataChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
	{
		ReadMaterialControls();
	}

	void MainPage::BoneList_DropDownOpened(Platform::Object^ sender, Platform::Object^ e)
	{
		m_boneList->Items->Clear();
		int modelIndex = m_modelList->SelectedIndex;
		if (modelIndex < 0) return;
		gfx::ModelLoader& ml = *m_modelLoaders[modelIndex];
		for (unsigned i = 0; i < ml.BoneCount(); i++)
		{
			auto& b = ml.Bones(i);
			m_boneList->Items->Append(ref new String(b.name.c_str(), b.name.length()));
		}
	}

	void MainPage::BoneList_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
	{
		FillBoneControls();
	}
	void MainPage::BoneData_TextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
	{
		ReadBoneControls();
	}
	void MainPage::BoneData_ValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
	{
		ReadBoneControls();
	}
}


