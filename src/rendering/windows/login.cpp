#include "../../application.h"
#include "../../imgui_utils.h"

void Application::render_login() {
    ImGui::Begin(u8"Přihlášení");
    ImGui::Text("Username");
    ImGui::InputTextWithHint("##username", "username", &state.username);
    ImGui::Text("Password");
    if (state.show_password) {
        ImGui::InputTextWithHint("##password_clear", "password", &state.password);
    }
    else {
        ImGui::InputTextWithHint("##password", "password", &state.password, ImGuiInputTextFlags_Password);
    }
    ImGui::SameLine();
    if (ImGui::Button(state.show_password ? "Hide password" : "Show password")) state.show_password = !state.show_password;
    if (ImGui::Button(u8"Přihlásit se")) {
        state.password = "";
        state.username = "";
    }
    ImGui::End();
}