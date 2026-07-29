#pragma once

#pragma once

#include <memory>

#ifdef AVO_DEBUG
#if defined(AVO_PLATFORM_WINDOWS)
#define AVO_DEBUGBREAK() __debugbreak()
#elif defined(AVO_PLATFORM_LINUX)
#include <signal.h>
#define AVO_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define AVO_ENABLE_ASSERTS
#else
#define AVO_DEBUGBREAK()
#endif

#define AVO_EXPAND_MACRO(x) x
#define AVO_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define AVO_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace avocado {

	template<typename T>
	using scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr scope<T> create_scope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr ref<T> create_ref(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

#include "log.h"
#include "assert.h"