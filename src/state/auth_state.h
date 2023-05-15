#pragma once

#include <optional>
#include <string>

struct AuthState {
	AuthState();
	~AuthState();
	void init();

	std::optional<std::string> access_token;
	std::optional<std::string> refresh_token;
	std::optional<time_t> access_token_expires_at;
};