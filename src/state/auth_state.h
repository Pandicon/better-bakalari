#pragma once

#include <optional>
#include <string>

struct AuthState {
	AuthState();
	~AuthState();

	std::optional<std::string> access_token;
	std::optional<std::string> refresh_token;
	std::optional<int> access_token_expires_at;
};