#include <spdlog/sinks/stdout_color_sinks.h>

#include "log.h"

std::shared_ptr<spdlog::logger> Log::s_logger;

void Log::Init() {
#ifdef _BA_DEBUG
	spdlog::set_pattern("%^[%Y-%m-%d %T] [%l] %v (%s:%#, %!)%$"); // Coloured, [YYYY-MM-DD HH:MM:SS] [LEVEL] message (file:line, function)
#else
	spdlog::set_pattern("%^[%Y-%m-%d %T] [%l] %v%$"); // Coloured, [YYYY-MM-DD HH:MM:SS] [LEVEL] message
#endif
	s_logger = spdlog::stdout_color_mt("APP");
	s_logger->set_level(spdlog::level::trace);
}