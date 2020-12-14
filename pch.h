#pragma once

#if !defined(WINAPI_FAMILY)
#define WINAPI_FAMILY WINAPI_FAMILY_PC_APP
#endif

#if !defined(UNICODE) || !defined(_UNICODE)
#error "Compile with Unicode"
#endif

#if !defined(NDEBUG) && !defined(_DEBUG)
#error "Define either NDEBUG for Release or _DEBUG for Debug builds"
#endif

#if !defined(__cplusplus) || \
    !defined(_MSC_VER) && __cplusplus < 201703L || \
    defined(_MSC_VER) && _MSVC_LANG < 201703L
#error Compile with C++17 or later
#endif

#if defined(__cplusplus_winrt)
#error "Don't compiled with the /ZW switch"
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <windows.foundation.h>

#include <wrl.h>
#include <wrl\wrappers\corewrappers.h>

