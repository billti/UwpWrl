#include "pch.h"

#include "AppFramework.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

using namespace ABI::Windows::ApplicationModel::Core;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Web::Http;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// Initialize COM/WinRT
	CheckHR(RoInitialize(RO_INIT_MULTITHREADED));

	// CoreApplication is a static class, so need to get the factory
	ComPtr<ICoreApplication> coreApp;
	CheckHR(GetActivationFactory(HStringReference(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication).Get(), &coreApp));

	ComPtr<AppFramework> framework = Make<AppFramework>();
	CheckHR(coreApp->Run(framework.Get()));

	return 0;
}

