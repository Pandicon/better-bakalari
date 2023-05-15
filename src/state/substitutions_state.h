#pragma once

#include "../log.h"

#include <optional>
#include <string>
#include <vector>

struct Substitution {
	Substitution(std::string day, std::string subject, std::string hours, std::string change_type, std::string description, std::string time);
	~Substitution();

	bool operator<(const Substitution& rhs) const {
		if (day != rhs.day) {
			return day < rhs.day;
		}
		std::string lhs_hour_start = time.substr(0, time.find(" - "));
		if (lhs_hour_start.length() < 5) { // Time between 0:00 and 9:59
			lhs_hour_start = "0" + lhs_hour_start;
		}
		std::string rhs_hour_start = rhs.time.substr(0, rhs.time.find(" - "));
		if (rhs_hour_start.length() < 5) { // Time between 0:00 and 9:59
			rhs_hour_start = "0" + rhs_hour_start;
		}
		return lhs_hour_start < rhs_hour_start;
	};

	std::string day;
	std::string subject;
	std::string hours;
	std::string change_type;
	std::string description;
	std::string time;
};

struct SubstitutionDay {
	SubstitutionDay(std::string day, std::vector<Substitution> substitutions);
	~SubstitutionDay();

	std::string day;
	std::vector<Substitution> substitutions;
};

struct SubstitutionsState {
	SubstitutionsState();
	~SubstitutionsState();

	bool parse_from_json_string(std::string json_string);

	std::optional<std::string> api_response;
	std::vector<SubstitutionDay> substitution_days;
	bool just_reloaded;

	time_t last_loaded_timestamp;
	std::string last_loaded;

	bool load_automatically;
	int load_delay_seconds;
};