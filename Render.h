#pragma once

#include "pch.h"

// Direct 3D 11.4 shipped in 2015, so seems like a reasonable expectation now.
// Note is also transitively includes DXGI 1.5

#include <d3d11_4.h>
#include <d2d1_1.h>
#include <dwrite.h>

class Render {
public:
	Render(float dpi, HWND hwnd, IUnknown* pIUnk_core_window = nullptr);
	~Render();
	void Paint();

private:

	// Direct3D 11.4 includes:
	// - ID3D11Device5
	// - ID3D11VideoDevice2
	// - ID3D11VideoContext3
	// - ID3D11Multithread 
	// - IDXGIDevice4
	// - IDXGIFactory5
	// - IDXGISwapChain4
	// - IDXGIOutput5
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush_;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2d1_context_;
	Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory_;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain_;
};
