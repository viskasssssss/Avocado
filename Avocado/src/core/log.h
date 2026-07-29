#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace avocado
{
	class logger
	{
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& get_logger() { return s_logger; }
	private:
		static std::shared_ptr<spdlog::logger> s_logger;
	};
}

#define AVO_ERROR(...) ::avocado::logger::get_logger()->error(__VA_ARGS__)
#define AVO_WARN(...) ::avocado::logger::get_logger()->warn(__VA_ARGS__)
#define AVO_INFO(...) ::avocado::logger::get_logger()->info(__VA_ARGS__)
#define AVO_TRACE(...) ::avocado::logger::get_logger()->trace(__VA_ARGS__)
#define AVO_CRITICAL(...) ::avocado::logger::get_logger()->critical(__VA_ARGS__)