#pragma once

#include "pch.h"

#include <Windows.ApplicationModel.h>

class AppFramework : public Microsoft::WRL::RuntimeClass<
	ABI::Windows::ApplicationModel::Core::IFrameworkView,
	ABI::Windows::ApplicationModel::Core::IFrameworkViewSource> {
  public:
    // IFrameworkViewSource
    HRESULT __stdcall CreateView(ABI::Windows::ApplicationModel::Core::IFrameworkView** viewProvider) override;

    // IFrameworkView
    HRESULT __stdcall Initialize(ABI::Windows::ApplicationModel::Core::ICoreApplicationView* applicationView) override;
    HRESULT __stdcall SetWindow(ABI::Windows::UI::Core::ICoreWindow* window) override;
    HRESULT __stdcall Load(HSTRING entryPoint) override;
    HRESULT __stdcall Run(void) override;
    HRESULT __stdcall Uninitialize(void) override;

  private:
    Microsoft::WRL::ComPtr<ABI::Windows::UI::Core::ICoreWindow> coreWindow_;
    EventRegistrationToken activatedToken_ = {};
    EventRegistrationToken closedToken_ = {};
    EventRegistrationToken pressedToken_ = {};
    bool isWindowClosed_ = false;
    float currentDpi_ = 0.0f;
};

