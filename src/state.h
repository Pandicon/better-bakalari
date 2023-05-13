#pragma once

#include <string>
#include <imgui.h>

#include "./state/login_state.h"

struct State
{
	State();
	~State();

	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;

	LoginState login;

	float fps_clamp;
};

