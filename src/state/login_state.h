#pragma once

#include <optional>
#include <string>

struct LoginState {
	LoginState();
	~LoginState();

	bool show_password;

	std::string api_url;
	std::string password;
	std::string username;
	std::optional<std::string> api_response;
};