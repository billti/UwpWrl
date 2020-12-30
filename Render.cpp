#include "pch.h"

#include "render.h"

using namespace Microsoft::WRL;

// TODO: Video rendering: https://docs.microsoft.com/en-us/windows/win32/medfound/direct3d-11-video-apis

Render::Render(float dpi, HWND hwnd, IUnknown* pIUnk_core_window) {
    // Note: Useful example at https://docs.microsoft.com/en-us/windows/win32/direct2d/devices-and-device-contexts
    
#if (WINAPI_FAMILY==WINAPI_FAMILY_PC_APP)
    // For UWP apps, only the core window should be passed
    if (pIUnk_core_window == nullptr || hwnd != NULL) {
        abort();
    }
#else
    // For Win32 apps, only the HWND should be passed
    if (pIUnk_core_window != nullptr || hwnd == NULL) {
        abort();
    }
#endif

    // === Create the D3D/DXGI Device, Adapter, and Factory ===

    UINT creation_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
    creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL feature_level[] = { D3D_FEATURE_LEVEL_11_1 };
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        creation_flags, feature_level, 1,
        D3D11_SDK_VERSION, &device, nullptr, &context);
    CheckHR(hr);

    ComPtr<IDXGIDevice2> dxgi_device2;
    hr = device.As(&dxgi_device2);
    CheckHR(hr);

    ComPtr<IDXGIAdapter> dxgi_adapter;
    hr = dxgi_device2->GetAdapter(&dxgi_adapter);
    CheckHR(hr);

    ComPtr<IDXGIFactory2> dxgi_factory2;
    hr = dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), &dxgi_factory2);
    CheckHR(hr);

    // === Create the swapchain and get the buffer as a D3D texture ===

    ComPtr<IDXGISwapChain1> swap_chain;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = { 0 };
    swap_chain_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    if (hwnd != NULL) {
        hr = dxgi_factory2->CreateSwapChainForHwnd(dxgi_device2.Get(), hwnd,
            &swap_chain_desc, nullptr, nullptr, &swap_chain);
        CheckHR(hr);
    }
    else {
        hr = dxgi_factory2->CreateSwapChainForCoreWindow(dxgi_device2.Get(), pIUnk_core_window,
            &swap_chain_desc, nullptr, &swap_chain);
        CheckHR(hr);
    }

    // Get the width, height, etc. of the swap chain created
    ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
    hr = swap_chain->GetDesc1(&swap_chain_desc);
    CheckHR(hr);

    // === Create a view on the D3D texture and map the D3D viewport to it ===

    // Note: The next few lines are not needed to use Direct2D, only if using Direct3D rendering

    ComPtr<ID3D11Texture2D> texture2d;
    hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &texture2d);
    CheckHR(hr);

    ComPtr<ID3D11RenderTargetView> render_target_view;
    hr = device->CreateRenderTargetView(texture2d.Get(), nullptr, &render_target_view);
    CheckHR(hr);

    context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), nullptr);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = (float)swap_chain_desc.Width;
    viewport.Height = (float)swap_chain_desc.Height;
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);


    // === D2D stuff ===

    D2D1_FACTORY_OPTIONS d2d_options;
    d2d_options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    ComPtr<ID2D1Factory1> d2d1_factory;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2d_options, d2d1_factory.ReleaseAndGetAddressOf());
    CheckHR(hr);

    ComPtr<ID2D1Device> d2d1_device;
    hr = d2d1_factory->CreateDevice(dxgi_device2.Get(), &d2d1_device);
    CheckHR(hr);

    ComPtr<ID2D1DeviceContext> d2d1_context;
    hr = d2d1_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d1_context);
    CheckHR(hr);

    D2D1_BITMAP_PROPERTIES1 d2d1_bitmap_props = {};
    d2d1_bitmap_props.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    d2d1_bitmap_props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    d2d1_bitmap_props.dpiX = dpi;
    d2d1_bitmap_props.dpiY = dpi;
    d2d1_bitmap_props.bitmapOptions = D2D1_BITMAP_OPTIONS_CANNOT_DRAW | D2D1_BITMAP_OPTIONS_TARGET;
    d2d1_bitmap_props.colorContext = nullptr; // TODO

    ComPtr<IDXGISurface> dxgi_surface;
    hr = swap_chain->GetBuffer(0, __uuidof(IDXGISurface), &dxgi_surface);
    CheckHR(hr);

    ComPtr<ID2D1Bitmap1> d2d1_bitmap;
    hr = d2d1_context->CreateBitmapFromDxgiSurface(dxgi_surface.Get(), d2d1_bitmap_props, &d2d1_bitmap);
    CheckHR(hr);

    d2d1_context->SetTarget(d2d1_bitmap.Get());

    ComPtr<ID2D1SolidColorBrush> solid_brush;
    // Struct with RGBA float components
    hr = d2d1_context->CreateSolidColorBrush(D2D1::ColorF(0x00CC6633), &solid_brush);
    CheckHR(hr);

    ComPtr<IDWriteFactory> dwrite_factory;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &dwrite_factory);
    CheckHR(hr);

    // Assign to the class instances if all succeeded
    this->swap_chain_ = swap_chain;
    this->d2d1_context_ = d2d1_context;
    this->brush_ = solid_brush;
    this->dwrite_factory_ = dwrite_factory;
}

Render::~Render() = default;

void Render::Paint() {
    HRESULT hr = S_OK;
    d2d1_context_->BeginDraw();

    // Standard D2D rectangle
    D2D1_ROUNDED_RECT round_rect = {};
    round_rect.rect = D2D1::RectF(30.0f, 30.0f, 110.0f, 107.0f);
    round_rect.radiusX = 10;
    round_rect.radiusY = 10;
    d2d1_context_->DrawRoundedRectangle(round_rect, brush_.Get(), 15.0f);

    // === DirectWrite stuff ===
    wchar_t text[] = L"Hello, world!";
    UINT32 ch_count = ARRAYSIZE(text);

    ComPtr<IDWriteTextFormat> text_format;
    hr = dwrite_factory_->CreateTextFormat(L"Harrington", nullptr,
        DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
        80.0f, L"en-us", &text_format);
    CheckHR(hr);

    hr = text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    CheckHR(hr);
    hr = text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    CheckHR(hr);

    ComPtr<ID2D1SolidColorBrush> font_brush;
    // Struct with RGBA float components
    hr = d2d1_context_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GhostWhite, 0.9f), &font_brush);

    CheckHR(hr);
    D2D1_RECT_F text_rect = D2D1::RectF(55.0f, 30.0f, 600.0f, 120.0f);
    d2d1_context_->DrawText(text, ch_count, text_format.Get(), text_rect, font_brush.Get());

    // A D2D gradient ellipse
    ComPtr<ID2D1GradientStopCollection> gradient_stops;
    D2D1_GRADIENT_STOP gradientStops[2];
    gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::DeepSkyBlue);
    gradientStops[0].position = 0.0f;
    gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Black, 0.9f);
    gradientStops[1].position = 1.0f;

    hr = d2d1_context_->CreateGradientStopCollection(
        gradientStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP,
        &gradient_stops);
    CheckHR(hr);

    ComPtr<ID2D1RadialGradientBrush> radial_gradient_brush;
    hr = d2d1_context_->CreateRadialGradientBrush(
        D2D1::RadialGradientBrushProperties(D2D1::Point2F(300, 150),
            D2D1::Point2F(0, 0), 100, 100),
        gradient_stops.Get(), &radial_gradient_brush);
    CheckHR(hr);

    D2D1_ELLIPSE ellipse;
    ellipse.point.x = 275;
    ellipse.point.y = 150;
    ellipse.radiusX = 75;
    ellipse.radiusY = 75;

    d2d1_context_->FillEllipse(ellipse, radial_gradient_brush.Get());

    // === End the drawing and present the swapchain ===

    hr = d2d1_context_->EndDraw();
    CheckHR(hr);

    DXGI_PRESENT_PARAMETERS present_params = { 0 };
    hr = swap_chain_->Present1(1, 0, &present_params);
    CheckHR(hr);
}

