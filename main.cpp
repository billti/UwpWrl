#include "pch.h"

#include "AppFramework.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

using namespace ABI::Windows::ApplicationModel::Core;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Web::Http;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	// Initialize COM/WinRT
	HRESULT hr = RoInitialize(RO_INIT_MULTITHREADED);
	CheckHR(hr);

	// CoreApplication is a static class, so need to get the factory
	ComPtr<ICoreApplication> coreApp;
	hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication).Get(), &coreApp);
	CheckHR(hr);

	ComPtr<AppFramework> framework = Make<AppFramework>();
	hr = coreApp->Run(framework.Get());
	CheckHR(hr);

	/* Below is example code for using async operations */

	/*
	// Create a URI for the web request
	// Need to get the ActivationFactory first, then create an instance of the object
	ComPtr<IUriRuntimeClassFactory> uriClassFactory;
	hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Foundation_Uri).Get(), &uriClassFactory);
	CheckHR(hr);

	HString uri;
	hr = uri.Set(L"https://www.microsoft.com");
	CheckHR(hr);

	ComPtr<IUriRuntimeClass> uriClass;
	hr = uriClassFactory->CreateUri(uri.Get(), &uriClass);
	CheckHR(hr);

	using HttpResponseAsyncOp = IAsyncOperationWithProgress<HttpResponseMessage*, struct HttpProgress>;
	using HttpResponseAsyncOpCompleted = IAsyncOperationWithProgressCompletedHandler<ABI::Windows::Web::Http::HttpResponseMessage*, struct ABI::Windows::Web::Http::HttpProgress>;

	// Create a web request client. These can be created directly with RoActivateInstance
	ComPtr<IHttpClient> httpClient;
	hr = RoActivateInstance(HStringReference(RuntimeClass_Windows_Web_Http_HttpClient).Get(), &httpClient);
	CheckHR(hr);

	ComPtr<IAsyncOperationWithProgress<HttpResponseMessage*, struct HttpProgress>> getProgress;
	hr = httpClient->GetAsync(uriClass.Get(), &getProgress);
	CheckHR(hr);

	// Set the event and callback for when the request completes
	Event requestCompleted(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, WRITE_OWNER | EVENT_ALL_ACCESS));
	CheckHR(requestCompleted.IsValid() ? S_OK : E_FAIL);

	// Figuring out the callback types and creating one is a pain, but here it it...
	auto callback = Callback<Implements<RuntimeClassFlags<Delegate>, HttpResponseAsyncOpCompleted, FtmBase>>
		([&requestCompleted](HttpResponseAsyncOp* pMessage, AsyncStatus status) -> HRESULT
			{
				// This callback will run on a worker thread

				if (status == AsyncStatus::Completed) {
					ComPtr<IHttpResponseMessage> response;
					HRESULT hr = pMessage->GetResults(&response);
					CheckHR(hr);
					HttpStatusCode statusCode;
					hr = response->get_StatusCode(&statusCode);
					CheckHR(hr);
					SetEvent(requestCompleted.Get());
				}
				return S_OK;
			}
	    );

	hr = getProgress->put_Completed(callback.Get());
	CheckHR(hr);

	// Wait on the event to complete before exiting
	WaitForSingleObjectEx(requestCompleted.Get(), INFINITE, FALSE);

	*/

	RoUninitialize();
	return 0;
}

