#include "pch.h"

namespace UwpWrl {
	using namespace Microsoft::WRL;
	using namespace Microsoft::WRL::Wrappers;
	using namespace ABI::Windows::Foundation;
	using namespace ABI::Windows::System::Threading;

	void DoWork() {
		// Get the threadpool statics
		ComPtr<IThreadPoolStatics> threadPool;
		CheckHR(GetActivationFactory(HStringReference(RuntimeClass_Windows_System_Threading_ThreadPool).Get(), &threadPool));

		// Create the WorkItemHandler that does the actual work
		// Note: Using the 'Delegate' flags and implementing FtmBase will let this run on a worker thread,
		// rather than queing back to the same 'apartment'/thread.
		auto workItem = Callback<Implements<RuntimeClassFlags<Delegate>, IWorkItemHandler, FtmBase>>(
			[](IAsyncAction* asyncAction) -> HRESULT {
				OutputDebugStringW(L"Running the work item");
				// Check if it's been cancelled already.
				// Note: IAsyncInfo is actually a required interface, not a base of IAsyncAction (as the docs state),
				// so you need to fetch it.
				ComPtr<IAsyncInfo> asyncInfo;
				asyncAction->QueryInterface(asyncInfo.GetAddressOf());

				AsyncStatus status;
				CheckHR(asyncInfo->get_Status(&status));
				if (status == AsyncStatus::Canceled) return S_OK;

				// TODO: some work
				return S_OK;
			}
		);


		// Queue the work
		ComPtr<IAsyncAction> asyncAction;
		CheckHR(threadPool->RunAsync(workItem.Get(), &asyncAction));

		// Set the completed handler. This will run back on the window's thread/apartment.
		auto completedHandler = Callback<IAsyncActionCompletedHandler>([](IAsyncAction* action, AsyncStatus status)->HRESULT {
			OutputDebugStringW(L"Async work done!");
			return S_OK;
		});
		CheckHR(asyncAction->put_Completed(completedHandler.Get()));
	}
}

	/* Below is example code for using async operations */

	/*
	#include <windows.web.http.h>

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
