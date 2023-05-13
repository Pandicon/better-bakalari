#include "auth_state.h"

AuthState::AuthState() {
	access_token = std::nullopt;
	refresh_token = std::nullopt;
	access_token_expires_at = std::nullopt;
}

AuthState::~AuthState() {};