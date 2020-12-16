#pragma once

#if !defined(WINAPI_FAMILY)
#error This program must be compiled as a Windows Store app
#endif

#if !defined(UNICODE) || !defined(_UNICODE)
#error Compile with Unicode
#endif

#if !defined(NDEBUG) && !defined(_DEBUG)
#error Define either NDEBUG for Release or _DEBUG for Debug builds
#endif

#if !defined(__cplusplus) || \
    !defined(_MSC_VER) && __cplusplus < 201703L || \
    defined(_MSC_VER) && _MSVC_LANG < 201703L
#error Compile with C++17 or later
#endif

#if defined(__cplusplus_winrt)
#error Don't compiled with the C++/CX extensions (i.e. /ZW switch)
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <windows.foundation.h>
#include <windows.system.threading.h>
#include <windows.web.http.h>

#include <wrl.h>
#include <wrl\wrappers\corewrappers.h>

#include <stdlib.h>

// Quick and dirty check to bring up the debugger in debug builds
#define CheckHR(hr) do { if (FAILED(hr)) { abort(); } } while (0)

