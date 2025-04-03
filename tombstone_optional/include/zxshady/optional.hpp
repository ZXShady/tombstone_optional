#pragma once
#if defined(_MSVC_LANG) && __cplusplus != _MSVC_LANG
#error _MSVC_LANG must be equal to __cplusplus please enable /Zc:__cplusplus
#endif

#if __cplusplus >= 202002L
#include "optional_cpp20.hpp"
#else
#error ZXShady tombstone_optional is only usable in C++20 currently
#endif