#include "pch.h"
#include "defines.h"
#include "log.h"

namespace avocado
{
	ref<spdlog::logger> logger::s_logger;

	void logger::init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		s_logger = spdlog::stdout_color_mt("APP");
		s_logger->set_level(spdlog::level::trace);
	}
}