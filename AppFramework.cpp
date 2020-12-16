#include "pch.h"
#include "AppFramework.h"

using namespace ABI::Windows::ApplicationModel::Core;
using namespace ABI::Windows::ApplicationModel::Activation;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Core;

using namespace Microsoft::WRL;

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
	return S_OK;
}

HRESULT __stdcall AppFramework::Load(HSTRING entryPoint) {
	return S_OK;
}

HRESULT __stdcall AppFramework::Run() {
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

