﻿//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------
#include "pch.h"

#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BINDING_DEBUG_OUTPUT
extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
#endif

#include "MainPage.xaml.h"

void ::Converter::MainPage::InitializeComponent()
{
    if (_contentLoaded)
    {
        return;
    }
    _contentLoaded = true;
    ::Windows::Foundation::Uri^ resourceLocator = ref new ::Windows::Foundation::Uri(L"ms-appx:///MainPage.xaml");
    ::Windows::UI::Xaml::Application::LoadComponent(this, resourceLocator, ::Windows::UI::Xaml::Controls::Primitives::ComponentResourceLocation::Application);
}

void ::Converter::MainPage::Connect(int __connectionId, ::Platform::Object^ __target)
{
    switch (__connectionId)
    {
    case 2:
        {
            this->m_swapChainPanel = safe_cast<::Windows::UI::Xaml::Controls::SwapChainPanel^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::SwapChainPanel^>(this->m_swapChainPanel))->DragOver += ref new ::Windows::UI::Xaml::DragEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::DragEventArgs^))&MainPage::OnDragOver);
            (safe_cast<::Windows::UI::Xaml::Controls::SwapChainPanel^>(this->m_swapChainPanel))->Drop += ref new ::Windows::UI::Xaml::DragEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::DragEventArgs^))&MainPage::OnDrop);
            (safe_cast<::Windows::UI::Xaml::Controls::SwapChainPanel^>(this->m_swapChainPanel))->PointerPressed += ref new ::Windows::UI::Xaml::Input::PointerEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Input::PointerRoutedEventArgs^))&MainPage::OnPointerPressed);
            (safe_cast<::Windows::UI::Xaml::Controls::SwapChainPanel^>(this->m_swapChainPanel))->PointerMoved += ref new ::Windows::UI::Xaml::Input::PointerEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Input::PointerRoutedEventArgs^))&MainPage::OnPointerMoved);
            (safe_cast<::Windows::UI::Xaml::Controls::SwapChainPanel^>(this->m_swapChainPanel))->PointerWheelChanged += ref new ::Windows::UI::Xaml::Input::PointerEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Input::PointerRoutedEventArgs^))&MainPage::OnPointerWheelChanged);
        }
        break;
    case 3:
        {
            this->m_menuPanel = safe_cast<::Windows::UI::Xaml::Controls::StackPanel^>(__target);
        }
        break;
    case 4:
        {
            this->m_modelList = safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(this->m_modelList))->DropDownOpened += ref new ::Windows::Foundation::EventHandler<::Platform::Object^>(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Platform::Object^))&MainPage::ModelList_DropDownOpened);
            (safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(this->m_modelList))->SelectionChanged += ref new ::Windows::UI::Xaml::Controls::SelectionChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::SelectionChangedEventArgs^))&MainPage::ModelList_SelectionChanged);
        }
        break;
    case 5:
        {
            this->m_materialList = safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(this->m_materialList))->DropDownOpened += ref new ::Windows::Foundation::EventHandler<::Platform::Object^>(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Platform::Object^))&MainPage::MaterialList_DropDownOpened);
            (safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(this->m_materialList))->SelectionChanged += ref new ::Windows::UI::Xaml::Controls::SelectionChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::SelectionChangedEventArgs^))&MainPage::MaterialList_SelectionChanged);
        }
        break;
    case 6:
        {
            this->m_textureTextBox = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(__target);
        }
        break;
    case 7:
        {
            this->m_normalmapTextBox = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(__target);
        }
        break;
    case 8:
        {
            this->m_specularPowerSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_specularPowerSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::MaterialDataChanged);
        }
        break;
    case 9:
        {
            this->m_specularRedSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_specularRedSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::MaterialDataChanged);
        }
        break;
    case 10:
        {
            this->m_specularGreenSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_specularGreenSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::MaterialDataChanged);
        }
        break;
    case 11:
        {
            this->m_specularBlueSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_specularBlueSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::MaterialDataChanged);
        }
        break;
    case 12:
        {
            this->m_specularAlphaSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_specularAlphaSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::MaterialDataChanged);
        }
        break;
    case 13:
        {
            this->m_textureWeightSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_textureWeightSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::MaterialDataChanged);
        }
        break;
    case 14:
        {
            this->m_diffuseRedSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_diffuseRedSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::MaterialDataChanged);
        }
        break;
    case 15:
        {
            this->m_diffuseGreenSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_diffuseGreenSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::MaterialDataChanged);
        }
        break;
    case 16:
        {
            this->m_diffuseBlueSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_diffuseBlueSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::MaterialDataChanged);
        }
        break;
    case 17:
        {
            this->m_diffuseAlphaSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_diffuseAlphaSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::MaterialDataChanged);
        }
        break;
    case 18:
        {
            ::Windows::UI::Xaml::Controls::Button^ element18 = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Button^>(element18))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ExportBinButton_Click);
        }
        break;
    case 19:
        {
            ::Windows::UI::Xaml::Controls::Button^ element19 = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Button^>(element19))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ExportTextButton_Click);
        }
        break;
    case 20:
        {
            this->m_modelShownCheckBox = safe_cast<::Windows::UI::Xaml::Controls::CheckBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::CheckBox^>(this->m_modelShownCheckBox))->Checked += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ModelShownCheckBox_CheckChanged);
            (safe_cast<::Windows::UI::Xaml::Controls::CheckBox^>(this->m_modelShownCheckBox))->Unchecked += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::ModelShownCheckBox_CheckChanged);
        }
        break;
    case 21:
        {
            ::Windows::UI::Xaml::Controls::Button^ element21 = safe_cast<::Windows::UI::Xaml::Controls::Button^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Button^>(element21))->Click += ref new ::Windows::UI::Xaml::RoutedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::RoutedEventArgs^))&MainPage::DeleteButton_Click);
        }
        break;
    case 22:
        {
            this->m_bonePanel = safe_cast<::Windows::UI::Xaml::Controls::StackPanel^>(__target);
        }
        break;
    case 23:
        {
            this->m_boneList = safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(this->m_boneList))->DropDownOpened += ref new ::Windows::Foundation::EventHandler<::Platform::Object^>(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Platform::Object^))&MainPage::BoneList_DropDownOpened);
            (safe_cast<::Windows::UI::Xaml::Controls::ComboBox^>(this->m_boneList))->SelectionChanged += ref new ::Windows::UI::Xaml::Controls::SelectionChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::SelectionChangedEventArgs^))&MainPage::BoneList_SelectionChanged);
        }
        break;
    case 24:
        {
            this->m_toBoneTxTextBox = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(this->m_toBoneTxTextBox))->TextChanged += ref new ::Windows::UI::Xaml::Controls::TextChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::TextChangedEventArgs^))&MainPage::BoneData_TextChanged);
        }
        break;
    case 25:
        {
            this->m_toBoneTyTextBox = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(this->m_toBoneTyTextBox))->TextChanged += ref new ::Windows::UI::Xaml::Controls::TextChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::TextChangedEventArgs^))&MainPage::BoneData_TextChanged);
        }
        break;
    case 26:
        {
            this->m_toBoneTzTextBox = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(this->m_toBoneTzTextBox))->TextChanged += ref new ::Windows::UI::Xaml::Controls::TextChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::TextChangedEventArgs^))&MainPage::BoneData_TextChanged);
        }
        break;
    case 27:
        {
            this->m_toBoneRxSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_toBoneRxSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::BoneData_ValueChanged);
        }
        break;
    case 28:
        {
            this->m_toBoneRySlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_toBoneRySlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::BoneData_ValueChanged);
        }
        break;
    case 29:
        {
            this->m_toBoneRzSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_toBoneRzSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::BoneData_ValueChanged);
        }
        break;
    case 30:
        {
            this->m_transformTxTextBox = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(this->m_transformTxTextBox))->TextChanged += ref new ::Windows::UI::Xaml::Controls::TextChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::TextChangedEventArgs^))&MainPage::BoneData_TextChanged);
        }
        break;
    case 31:
        {
            this->m_transformTyTextBox = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(this->m_transformTyTextBox))->TextChanged += ref new ::Windows::UI::Xaml::Controls::TextChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::TextChangedEventArgs^))&MainPage::BoneData_TextChanged);
        }
        break;
    case 32:
        {
            this->m_transformTzTextBox = safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::TextBox^>(this->m_transformTzTextBox))->TextChanged += ref new ::Windows::UI::Xaml::Controls::TextChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::TextChangedEventArgs^))&MainPage::BoneData_TextChanged);
        }
        break;
    case 33:
        {
            this->m_transformRxSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_transformRxSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::BoneData_ValueChanged);
        }
        break;
    case 34:
        {
            this->m_transformRySlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_transformRySlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::BoneData_ValueChanged);
        }
        break;
    case 35:
        {
            this->m_transformRzSlider = safe_cast<::Windows::UI::Xaml::Controls::Slider^>(__target);
            (safe_cast<::Windows::UI::Xaml::Controls::Slider^>(this->m_transformRzSlider))->ValueChanged += ref new ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventHandler(this, (void (::Converter::MainPage::*)
                (::Platform::Object^, ::Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^))&MainPage::BoneData_ValueChanged);
        }
        break;
    }
    _contentLoaded = true;
}

::Windows::UI::Xaml::Markup::IComponentConnector^ ::Converter::MainPage::GetBindingConnector(int __connectionId, ::Platform::Object^ __target)
{
    __connectionId;         // unreferenced
    __target;               // unreferenced
    return nullptr;
}

#pragma warning(pop)


