﻿//------------------------------------------------------------------------------
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//------------------------------------------------------------------------------

#include "pch.h"
#include "App.xaml.h"


#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();
#endif


#if (defined(_M_IX86) || defined(_M_AMD64)) && !defined(_VSDESIGNER_DONT_LOAD_AS_DLL)
#if defined(_M_IX86)
#pragma comment(linker, "/EXPORT:DllGetActivationFactory=_DllGetActivationFactory@8,PRIVATE")
#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow@0,PRIVATE")
#pragma comment(linker, "/EXPORT:VSDesignerDllMain=_VSDesignerDllMain@12,PRIVATE")
#pragma comment(linker, "/INCLUDE:___refMTAThread")
#elif defined(_M_AMD64)
#pragma comment(linker, "/EXPORT:DllGetActivationFactory=DllGetActivationFactory,PRIVATE")
#pragma comment(linker, "/EXPORT:DllCanUnloadNow,PRIVATE")
#pragma comment(linker, "/EXPORT:VSDesignerDllMain,PRIVATE")
#pragma comment(linker, "/INCLUDE:__refMTAThread")
#endif

extern int __abi___threading_model;

extern "C"
{
    int __stdcall _DllMainCRTStartup(void* hinstDLL, unsigned long fdwReason, void** lpvReserved);

    int __stdcall VSDesignerDllMain(void* hinstDLL, unsigned long fdwReason, void** lpvReserved)
    {
        __abi___threading_model = _VCCORLIB_THREADING_MODEL_OVERRIDE;
        return _DllMainCRTStartup(hinstDLL, fdwReason, lpvReserved);
    }
}

#endif // (defined(_M_IX86) || defined(_M_AMD64)) && !defined(_VSDESIGNER_DONT_LOAD_AS_DLL)

void ::TowerDefense::App::InitializeComponent()
{
#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
    UnhandledException += ref new ::Windows::UI::Xaml::UnhandledExceptionEventHandler(
        [](::Platform::Object^ sender, ::Windows::UI::Xaml::UnhandledExceptionEventArgs^ e)
        {
            (void)sender; // Unused parameter
            if (IsDebuggerPresent())
            {
                ::Platform::String^ errorMessage = e->Message;
                __debugbreak();
            }
        });
#endif
}

::Windows::UI::Xaml::Markup::IXamlType^ ::TowerDefense::App::GetXamlType(::Windows::UI::Xaml::Interop::TypeName type)
{
    return _AppProvider->GetXamlTypeByType(type);
}

::Windows::UI::Xaml::Markup::IXamlType^ ::TowerDefense::App::GetXamlType(::Platform::String^ fullName)
{
    return _AppProvider->GetXamlTypeByName(fullName);
}

::Platform::Array<::Windows::UI::Xaml::Markup::XmlnsDefinition>^ ::TowerDefense::App::GetXmlnsDefinitions()
{
    return ref new ::Platform::Array<::Windows::UI::Xaml::Markup::XmlnsDefinition>(0);
}

::XamlTypeInfo::InfoProvider::XamlTypeInfoProvider^ ::TowerDefense::App::_AppProvider::get()
{
    if (__provider == nullptr)
    {
        __provider = ref new ::XamlTypeInfo::InfoProvider::XamlTypeInfoProvider();
    }
    return __provider;
}

#ifndef DISABLE_XAML_GENERATED_MAIN
int __cdecl main(::Platform::Array<::Platform::String^>^ args)
{
    (void)args; // Unused parameter
    ::Windows::UI::Xaml::Application::Start(ref new ::Windows::UI::Xaml::ApplicationInitializationCallback(
        [](::Windows::UI::Xaml::ApplicationInitializationCallbackParams^ p) {
            (void)p; // Unused parameter
            auto app = ref new ::TowerDefense::App();
        }));
}
#endif


