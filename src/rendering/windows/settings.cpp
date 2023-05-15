#define CPPHTTPLIB_OPENSSL_SUPPORT
#define __STDC_WANT_LIB_EXT1__ 1

#include "../../application.h"

void Application::render_settings() {
    if (!ImGui::Begin("Settings", &state.show_settings_window)) {
        ImGui::End();
        return;
    }
    if (ImGui::CollapsingHeader("Substitutions")) {
        ImGui::PushItemWidth(100);
        ImGui::Checkbox("Automatically reload substitutions", &state.substitutions.load_automatically);
        ImGui::DragInt("Substitutions reload delay (seconds)", &state.substitutions.load_delay_seconds, 0.5f, 0, INT_MAX);
        ImGui::PopItemWidth();
    }
    ImGui::End();
}