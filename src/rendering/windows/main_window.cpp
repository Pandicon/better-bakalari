#include "../../application.h"

void Application::render_main_window() {
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Substitutions")) {
            ImGui::CloseCurrentPopup();
            state.show_substitutions_window = true;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings")) {
            ImGui::CloseCurrentPopup();
            state.show_settings_window = true;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}