#include "login_state.h"

LoginState::LoginState() {
	show_password = false;

	api_url = "";
	username = "";
	password = "";
	api_response = std::nullopt;
};

LoginState::~LoginState() {};