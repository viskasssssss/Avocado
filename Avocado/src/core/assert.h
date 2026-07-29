#pragma once

#include "defines.h"
#include <filesystem>

#ifdef AVO_ENABLE_ASSERTS
#define AVO_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { AVO##type##ERROR(msg, __VA_ARGS__); AVO_DEBUGBREAK(); } }
#define AVO_INTERNAL_ASSERT_WITH_MSG(type, check, ...) AVO_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define AVO_INTERNAL_ASSERT_NO_MSG(type, check) AVO_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", AVO_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define AVO_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define AVO_INTERNAL_ASSERT_GET_MACRO(...) AVO_EXPAND_MACRO( AVO_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, AVO_INTERNAL_ASSERT_WITH_MSG, AVO_INTERNAL_ASSERT_NO_MSG) )

#define AVO_ASSERT(...) AVO_EXPAND_MACRO( AVO_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#else
#define AVO_ASSERT(...)
#endif