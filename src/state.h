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
	std::string example_string;
};

