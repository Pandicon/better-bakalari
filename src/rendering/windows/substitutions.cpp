#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <chrono>
#include <fstream>
#include <httplib.h>
#include <iostream>
#include <json/json.h>

#include "../../application.h"
#include "../../log.h"
#include "../../imgui_utils.h"

void Application::render_substitutions() {
    ImGui::Begin("Substitutions");
    for (auto substitutions_day : state.substitutions.substitution_days) {
        if (state.substitutions.just_reloaded) {
            ImGui::SetNextItemOpen(true);
        }
        if (ImGui::CollapsingHeader(substitutions_day.day.c_str())) {
            for (auto substitution : substitutions_day.substitutions) {
                ImGui::Text((substitution.hours + " | " + substitution.time).c_str());
                ImGui::Text(substitution.description.c_str());
                ImGui::Separator();
            }
        }
    }
    if (state.substitutions.just_reloaded) {
        state.substitutions.just_reloaded = false;
    }
    if (ImGui::Button("Load")) {
        httplib::SSLClient client(state.login.api_url);
        client.set_ca_cert_path("./resources/ca-bundle.crt");
        client.set_connection_timeout(0, 300000); // 300 milliseconds
        client.set_read_timeout(5, 0); // 5 seconds
        client.set_write_timeout(5, 0); // 5 seconds
        client.set_bearer_token_auth(state.auth.access_token.value());

        auto res = client.Get("/api/3/substitutions", { {"Content-Type", "application/x-www-form-urlencoded"} }, [](uint64_t len, uint64_t total) {
            BA_DEBUG("{} / {} bytes => {}% complete\n",
                len, total,
                (int)(len * 100 / total));
            return true; // return 'false' if you want to cancel the request.
           });
        if (res) {
            if (res->status == 200) {
                bool parse_success = state.substitutions.parse_from_json_string(res->body);
                if(!parse_success) {
                    state.substitutions.api_response.emplace("Something went wrong while parsing the response");
                } else {
                    state.substitutions.just_reloaded = true;
                }
            } else {
                state.substitutions.api_response.emplace("Something went wrong while getting the response: " + res->body);
            }
        }
        else {
            auto err = res.error();
            state.substitutions.api_response.emplace("HTTP error: " + httplib::to_string(err));
        }
    }
    if (state.substitutions.api_response.has_value()) {
        ImGui::TextWrapped(state.login.api_response.value().c_str());
    }
    ImGui::End();
}