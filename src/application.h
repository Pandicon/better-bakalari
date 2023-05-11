#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "state.h"

class Application {
public:
	Application();
	~Application();

	void Init(GLFWwindow* window, const char* glsl_version);
	virtual void Update();
	void Render();
	void Shutdown();

	ImGuiIO& io;
	State state;
};