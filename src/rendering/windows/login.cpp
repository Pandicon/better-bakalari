#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <httplib.h>
#include <iostream>

#include "../../application.h"
#include "../../imgui_utils.h"

void Application::render_login() {
    ImGui::Begin(u8"Přihlášení");
    ImGui::Text("Server URL");
    ImGui::InputTextWithHint("##server_url", "server url", &state.api_url);
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
        httplib::SSLClient cli(state.api_url);
        cli.set_ca_cert_path("./resources/ca-bundle.crt");
        cli.set_connection_timeout(0, 300000); // 300 milliseconds
        cli.set_read_timeout(5, 0); // 5 seconds
        cli.set_write_timeout(5, 0); // 5 seconds

        auto res = cli.Post("/api/login", "client_id=ANDR&grant_type=password&username=" + state.username + "&password=" + state.password, "application/x-www-form-urlencoded");

        if (res) {
            std::cout << res << std::endl;
            std::cout << "Status: " << res->status << std::endl;
            std::cout << "Body: " << res->body << std::endl;
        }
        else {
            auto err = res.error();
            std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
        }

        /*
        state.password = "";
        state.username = "";
        */
    }
    ImGui::End();
}