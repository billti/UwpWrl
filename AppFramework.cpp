#include "pch.h"
#include "AppFramework.h"
#include "Render.h"

#include <windows.Graphics.Display.h>

using namespace ABI::Windows::ApplicationModel::Core;
using namespace ABI::Windows::ApplicationModel::Activation;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Graphics::Display;
using namespace ABI::Windows::UI::Core;

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

namespace UwpWrl {
	void DoWork();
}

HRESULT __stdcall AppFramework::CreateView(IFrameworkView** viewProvider) {
	*viewProvider = static_cast<IFrameworkView*>(this);
	return S_OK;
}

HRESULT __stdcall AppFramework::Initialize(ICoreApplicationView* applicationView) {
	using ActivatedHandlerType = ITypedEventHandler<CoreApplicationView*, IActivatedEventArgs*>;

	ComPtr<ActivatedHandlerType> activatedCallback = Callback<ActivatedHandlerType>([this](ICoreApplicationView*, IActivatedEventArgs*) -> HRESULT {
		return this->coreWindow_->Activate();
	});

	isWindowClosed_ = false;
	CheckHR(applicationView->add_Activated(activatedCallback.Get(), &activatedToken_));
	return S_OK;
}

HRESULT __stdcall AppFramework::SetWindow(ICoreWindow* window) {
	using ClosedHandlerType = ITypedEventHandler<CoreWindow*, CoreWindowEventArgs*>;

	this->coreWindow_ = window;
	ComPtr<ClosedHandlerType> closedCallback = Callback<ClosedHandlerType>([this](ICoreWindow*, ICoreWindowEventArgs*) -> HRESULT {
		isWindowClosed_ = true;
		return S_OK;
	});
	CheckHR(window->add_Closed(closedCallback.Get(), &closedToken_));


	using PointerPressedHandlerType = ITypedEventHandler<CoreWindow*, PointerEventArgs*>;
	auto pressedHandler = Callback<PointerPressedHandlerType>([](ICoreWindow* window, IPointerEventArgs* pointer_args) -> HRESULT {
		OutputDebugString(L"Pointer pressed on the window");
		return S_OK;
	});

	window->add_PointerPressed(pressedHandler.Get(), &pressedToken_);
	return S_OK;
}

HRESULT __stdcall AppFramework::Load(HSTRING entryPoint) {
	ComPtr<IDisplayInformationStatics> display_info_statics;
	HRESULT hr = S_OK;
	hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Graphics_Display_DisplayInformation).Get(), &display_info_statics);
	CheckHR(hr);

	ComPtr<IDisplayInformation> display_info;
	hr = display_info_statics->GetForCurrentView(&display_info);
	CheckHR(hr);

	hr = display_info->get_LogicalDpi(&currentDpi_);
	CheckHR(hr);
	return S_OK;
}

HRESULT __stdcall AppFramework::Run() {
	UwpWrl::DoWork();

	Render renderer{currentDpi_, NULL, coreWindow_.Get(), };
	renderer.Paint();

	HRESULT hr = S_OK;
	ComPtr<ICoreDispatcher> dispatcher;
	CheckHR(coreWindow_->get_Dispatcher(&dispatcher));

	while (!isWindowClosed_) {
		hr = dispatcher->ProcessEvents(CoreProcessEventsOption::CoreProcessEventsOption_ProcessAllIfPresent);
	}

	return S_OK;
}

HRESULT __stdcall AppFramework::Uninitialize() {
	return S_OK;
}

