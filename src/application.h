#pragma once

#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "state.h"

class Application {
public:
	Application();
	~Application();

	void Init(GLFWwindow* window, const char* glsl_version);
	void NewFrame();
	virtual void Update();
	void Render();
	void Shutdown();

	ImGuiIO& io;
	State state;
	GLFWwindow* window;

private:
	void render_login();
	void render_settings();
	void render_substitutions();
	void update_substitutions();
	std::string get_date_time_string(time_t timestamp) const;
};