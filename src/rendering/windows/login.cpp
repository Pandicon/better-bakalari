#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <httplib.h>
#include <iostream>
#include <json/json.h>

#include "../../application.h"
#include "../../imgui_utils.h"

void Application::render_login() {
    ImGui::Begin("Login");
    ImGui::Text("Server URL");
    ImGui::InputTextWithHint("##server_url", "server url", &state.login.api_url);
    ImGui::Text("Username");
    ImGui::InputTextWithHint("##username", "username", &state.login.username);
    ImGui::Text("Password");
    if (state.login.show_password) {
        ImGui::InputTextWithHint("##password_clear", "password", &state.login.password);
    }
    else {
        ImGui::InputTextWithHint("##password", "password", &state.login.password, ImGuiInputTextFlags_Password);
    }
    ImGui::SameLine();
    if (ImGui::Button(state.login.show_password ? "Hide password" : "Show password")) state.login.show_password = !state.login.show_password;
    if (ImGui::Button("Login")) {
        httplib::SSLClient cli(state.login.api_url);
        cli.set_ca_cert_path("./resources/ca-bundle.crt");
        cli.set_connection_timeout(0, 300000); // 300 milliseconds
        cli.set_read_timeout(5, 0); // 5 seconds
        cli.set_write_timeout(5, 0); // 5 seconds

        auto res = cli.Post("/api/login", "client_id=ANDR&grant_type=password&username=" + state.login.username + "&password=" + state.login.password, "application/x-www-form-urlencoded");

        if (res) {
            std::cout << res << std::endl;
            std::cout << "Status: " << res->status << std::endl;
            std::cout << "Body: " << res->body << std::endl;
            if (res->status == 400) {
                state.login.api_response.emplace("Incorrect username or password");
                state.login.password = "";
                Json::Value body;
                Json::Reader reader;
                bool parsingSuccessful = reader.parse(res->body, body);
                if (parsingSuccessful) {
                    std::cout << body["error"] << std::endl;
                }
            } else if(res->status == 200) {
            
            }
        } else {
            auto err = res.error();
            std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
            state.login.api_response.emplace("HTTP error: " + httplib::to_string(err));
        }

        /*
        state.password = "";
        state.username = "";
        */
    }
    if (state.login.api_response.has_value()) {
        ImGui::TextWrapped(state.login.api_response.value().c_str());
    }
    ImGui::End();
}