#pragma once

#include <httplib.h>
#include <optional>
#include <string>
#include <imgui.h>

#include "./state/auth_state.h"
#include "./state/login_state.h"
#include "./state/substitutions_state.h"

struct State
{
	State();
	~State();
	void init();
	std::optional<std::string> get_access_token(std::string);
	bool is_access_token_valid();

	std::optional<std::string> api_url;

	bool show_settings_window;
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;

	AuthState auth;
	LoginState login;
	SubstitutionsState substitutions;

	float fps_clamp;
	time_t frame_timestamp;
};

