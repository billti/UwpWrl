#include "pch.h"

using Microsoft::WRL::ComPtr;
using Microsoft::WRL::Wrappers::HString;
using Microsoft::WRL::Wrappers::HStringReference;
using ABI::Windows::Foundation::GetActivationFactory;
using ABI::Windows::Foundation::IUriRuntimeClassFactory;
using ABI::Windows::Foundation::IUriRuntimeClass;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	HRESULT hr = RoInitialize(RO_INIT_MULTITHREADED);
	if (FAILED(hr)) return -1;

	ComPtr<IUriRuntimeClassFactory> uriClassFactory;
	hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Foundation_Uri).Get(), &uriClassFactory);

	HString uri;
	hr = uri.Set(L"https://www.microsoft.com");

	ComPtr<IUriRuntimeClass> uriClass;
	hr = uriClassFactory->CreateUri(uri.Get(), &uriClass);

	RoUninitialize();
	return 0;
}
