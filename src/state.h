#pragma once

#include <string>
#include <imgui.h>

class State
{
public:
	State();
	~State();

	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;
	std::string api_url;
	std::string password;
	std::string username;
	bool show_password;

	float fps_clamp;
};

