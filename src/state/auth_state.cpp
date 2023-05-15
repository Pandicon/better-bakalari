#include <fstream>
#include <iostream>
#include <string>

#include "auth_state.h"

AuthState::AuthState() {
	access_token = std::nullopt;
	refresh_token = std::nullopt;
	access_token_expires_at = std::nullopt;
}

AuthState::~AuthState() {};

void AuthState::init() {
	std::ifstream refresh_token_file("./token.b64");
	if (refresh_token_file.is_open()) {
		std::string line;
		std::getline(refresh_token_file, line);
		if (line.length() > 0) {
			refresh_token.emplace(line);
		}
	}
}