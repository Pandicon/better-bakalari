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

#ifdef _BA_DEBUG
#define BA_CRITICAL(...) Log::get_logger()->log(spdlog::source_loc{ __FILE__, __LINE__, static_cast<const char *>(__FUNCTION__) }, spdlog::level::critical, __VA_ARGS__)
#define BA_ERROR(...)    Log::get_logger()->log(spdlog::source_loc{ __FILE__, __LINE__, static_cast<const char *>(__FUNCTION__) }, spdlog::level::err, __VA_ARGS__)
#define BA_WARN(...)     Log::get_logger()->log(spdlog::source_loc{ __FILE__, __LINE__, static_cast<const char *>(__FUNCTION__) }, spdlog::level::warn, __VA_ARGS__)
#define BA_INFO(...)     Log::get_logger()->log(spdlog::source_loc{ __FILE__, __LINE__, static_cast<const char *>(__FUNCTION__) }, spdlog::level::info, __VA_ARGS__)
#define BA_DEBUG(...)    Log::get_logger()->log(spdlog::source_loc{ __FILE__, __LINE__, static_cast<const char *>(__FUNCTION__) }, spdlog::level::debug, __VA_ARGS__)
#define BA_TRACE(...)    Log::get_logger()->log(spdlog::source_loc{ __FILE__, __LINE__, static_cast<const char *>(__FUNCTION__) }, spdlog::level::trace, __VA_ARGS__)
#else
#define BA_CRITICAL(...) Log::get_logger()->critical(__VA_ARGS__)
#define BA_ERROR(...)    Log::get_logger()->error(__VA_ARGS__)
#define BA_WARN(...)     Log::get_logger()->warn(__VA_ARGS__)
#define BA_INFO(...)     Log::get_logger()->info(__VA_ARGS__)
#define BA_DEBUG(...)
#define BA_TRACE(...)
#endif

#if false
->log(spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, level, __VA_ARGS__)
#endif