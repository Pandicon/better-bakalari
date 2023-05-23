#include <time.h>

#include "date_time.h"

DateTime::DateTime(int _year, int _month, int _day, int _hour, int _minute, int _second) {
    year = _year;
    month = _month;
    day = _day;
    hour = _hour;
    minute = _minute;
    second = _second;
}

DateTime::~DateTime() {};

DateTime::DateTime(time_t timestamp) {
    std::tm time_info;
    localtime_s(&time_info, &timestamp);
    year = time_info.tm_year + 1900;
    month = time_info.tm_mon + 1;
    day = time_info.tm_mday;
    hour = time_info.tm_hour;
    minute = time_info.tm_min;
    second = time_info.tm_sec;
}

std::string DateTime::get_date_time_string(time_t timestamp, std::string date_time_join) const {
    DateTime date_time = DateTime::DateTime(timestamp);
    return (date_time.day < 10 ? "0" : "") + std::to_string(date_time.day) + "." +
        (date_time.month < 10 ? "0" : "") + std::to_string(date_time.month) + "." +
        std::to_string(date_time.year) + " " + (date_time_join != "" ? (date_time_join + " ") : "") +
        (date_time.hour < 10 ? "0" : "") + std::to_string(date_time.hour) + ":" +
        (date_time.minute < 10 ? "0" : "") + std::to_string(date_time.minute) + ":" +
        (date_time.second < 10 ? "0" : "") + std::to_string(date_time.second);
}

std::string DateTime::get_date_time_string(std::string date_time_join) const {
    return (day < 10 ? "0" : "") + std::to_string(day) + "." +
        (month < 10 ? "0" : "") + std::to_string(month) + "." +
        std::to_string(year) + " " + (date_time_join != "" ? (date_time_join + " ") : "") +
        (hour < 10 ? "0" : "") + std::to_string(hour) + ":" +
        (minute < 10 ? "0" : "") + std::to_string(minute) + ":" +
        (second < 10 ? "0" : "") + std::to_string(second);
}