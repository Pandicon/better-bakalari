#include <algorithm>
#include <json/json.h>

#include "../log.h"
#include "substitutions_state.h"

SubstitutionsState::SubstitutionsState() {
	api_response = std::nullopt;
	substitution_days = std::vector<SubstitutionDay>();
	just_reloaded = false;
}

SubstitutionsState::~SubstitutionsState() {};

bool SubstitutionsState::parse_from_json_string(std::string json_string) {
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

			if (substitutions_days.empty() || substitutions_days[subtitution_day_index].day != date_parsed) {
				substitutions_days.push_back(SubstitutionDay(date_parsed, std::vector<Substitution>()));
				subtitution_day_index += 1;
			}
			substitutions_days[subtitution_day_index].substitutions.push_back(substitution);
		}
		substitution_days = substitutions_days;
	}
	return parsingSuccessful;
};

SubstitutionDay::SubstitutionDay(std::string _day, std::vector<Substitution> _substitutions) {
	day = _day;
	substitutions = _substitutions;
};

SubstitutionDay::~SubstitutionDay() {};

Substitution::Substitution(std::string _day, std::string _subject, std::string _hours, std::string _change_type, std::string _description, std::string _time) {
	day = _day;
	subject = _subject;
	hours = _hours;
	change_type = _change_type;
	description = _description;
	time = _time;
};

Substitution::~Substitution() {};