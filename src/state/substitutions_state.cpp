#include <algorithm>
#include <json/json.h>

#include "../log.h"
#include "substitutions_state.h"

SubstitutionsState::SubstitutionsState() {
	api_response = std::nullopt;
	substitution_days = std::vector<SubstitutionDay>();
	just_reloaded = false;
	last_loaded_timestamp = 0;
	last_loaded = "";
	load_automatically = false;
	load_delay_seconds = 300;
}

SubstitutionsState::~SubstitutionsState() {};

std::optional<std::vector<SubstitutionDay>> SubstitutionsState::parse_from_json_string(std::string json_string) const {
	Json::Value body;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(json_string, body);
	if (parsingSuccessful) {
		std::vector<Substitution> substitutions;
		std::vector<SubstitutionDay> substitutions_days;
		for (auto change : body["Changes"]) {
			substitutions.push_back(Substitution(
				change["Day"].asString(),
				change["ChangeSubject"].asString(),
				change["Hours"].asString(),
				change["ChangeType"].asString(),
				change["Description"].asString(),
				change["Time"].asString()
			));
		}

		std::sort(substitutions.begin(), substitutions.end());

		int subtitution_day_index = -1;
		for (auto substitution : substitutions) {
			std::string date = substitution.day.substr(0, substitution.day.find("T"));
			std::string year = date.substr(0, date.find("-"));
			date.erase(0, date.find("-") + 1);
			std::string month = date.substr(0, date.find("-"));
			date.erase(0, date.find("-") + 1);
			std::string day = date.substr(0, date.find("-"));

			std::string date_parsed = day + "." + month + "." + year;
			std::string date_raw_parsed = year + "-" + month + "-" + day;

			if (substitutions_days.empty() || substitutions_days[subtitution_day_index].day != date_parsed) {
				substitutions_days.push_back(SubstitutionDay(date_parsed, date_raw_parsed, std::vector<Substitution>()));
				subtitution_day_index += 1;
			}
			substitutions_days[subtitution_day_index].substitutions.push_back(substitution);
		}
		return substitutions_days;
	}
	return std::nullopt;
};

SubstitutionDay::SubstitutionDay(std::string _day, std::string _day_raw, std::vector<Substitution> _substitutions) {
	day = _day;
	day_raw = _day_raw;
	substitutions = _substitutions;

	last_changes = std::vector<std::string>();
	last_change_timestamp = 0;
};

SubstitutionDay::~SubstitutionDay() {};

Substitution::Substitution(std::string _day, std::string _subject, std::string _hours, std::string _change_type, std::string _description, std::string _time) {
	day = _day;
	subject = _subject;
	hours = _hours;
	change_type = _change_type;
	description = _description;
	time = _time;

	last_change = "";
	last_change_timestamp = 0;
};

Substitution::~Substitution() {};

SubstitutionDayWithIndex::SubstitutionDayWithIndex(int _index, std::string _day, std::string _day_raw, std::vector<SubstitutionWithIndices> _substitutions) {
	index = _index;
	
	day = _day;
	day_raw = _day_raw;
	substitutions = _substitutions;

	last_changes = std::vector<std::string>();
	last_change_timestamp = 0;
};

SubstitutionDayWithIndex::~SubstitutionDayWithIndex() {};

SubstitutionWithIndices::SubstitutionWithIndices(int _day_index, int _index, std::string _day, std::string _subject, std::string _hours, std::string _change_type, std::string _description, std::string _time) {
	day_index = _day_index;
	index = _index;

	day = _day;
	subject = _subject;
	hours = _hours;
	change_type = _change_type;
	description = _description;
	time = _time;

	last_change = "";
	last_change_timestamp = 0;
};

SubstitutionWithIndices::SubstitutionWithIndices(int _day_index, int _index, const Substitution& _substitution) {
	day_index = _day_index;
	index = _index;

	day = _substitution.day;
	subject = _substitution.subject;
	hours = _substitution.hours;
	change_type = _substitution.change_type;
	description = _substitution.description;
	time = _substitution.time;

	last_change = _substitution.last_change;
	last_change_timestamp = _substitution.last_change_timestamp;
};

SubstitutionWithIndices::~SubstitutionWithIndices() {};