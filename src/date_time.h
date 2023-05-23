#pragma once

#include <string>

struct DateTime {
	int year; // The current year
	int month; // The current month (1-12)
	int day; // The current day within the month (1-31)
	int hour; // The current hour within the day (0-23)
	int minute; // The current minute within the hour
	int second; // The current second within the minute

	DateTime(time_t timestamp);
	DateTime(int year, int month, int day, int hour, int minute, int second);
	~DateTime();
	std::string get_date_time_string(time_t timestamp, std::string date_time_join = "") const;
	std::string get_date_time_string(std::string date_time_join = "") const;
};