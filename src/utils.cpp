#include <time.h>

#include "application.h"

std::string Application::get_date_time_string(time_t timestamp) const {
    std::tm time_info;
    localtime_s(&time_info, &timestamp);
    return (time_info.tm_mday < 10 ? "0" : "") + std::to_string(time_info.tm_mday) + "." +
        (time_info.tm_mon < 10 ? "0" : "") + std::to_string(time_info.tm_mon) + "." +
        std::to_string(1900 + time_info.tm_year) + " " +
        (time_info.tm_hour < 10 ? "0" : "") + std::to_string(time_info.tm_hour) + ":" +
        (time_info.tm_min < 10 ? "0" : "") + std::to_string(time_info.tm_min) + ":" +
        (time_info.tm_sec < 10 ? "0" : "") + std::to_string(time_info.tm_sec);
}