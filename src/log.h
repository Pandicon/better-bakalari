#pragma once

#include <memory>

#include <spdlog/spdlog.h>

class Log
{
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& get_logger() {
		return s_logger;
	};

private:
	static std::shared_ptr<spdlog::logger> s_logger;
};

#define BA_CRITICAL(...) Log::get_logger()->critical(__VA_ARGS__)
#define BA_ERROR(...)    Log::get_logger()->error(__VA_ARGS__)
#define BA_WARN(...)     Log::get_logger()->warn(__VA_ARGS__)
#define BA_INFO(...)     Log::get_logger()->info(__VA_ARGS__)
#define BA_DEBUG(...)    Log::get_logger()->debug(__VA_ARGS__)
#define BA_TRACE(...)    Log::get_logger()->trace(__VA_ARGS__)