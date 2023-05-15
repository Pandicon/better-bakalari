#define CPPHTTPLIB_OPENSSL_SUPPORT
#define __STDC_WANT_LIB_EXT1__ 1

#include <fstream>
#include <httplib.h>
#include <iostream>
#include <json/json.h>
#include <time.h>

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
    if (ImGui::Button("Load") || (state.substitutions.load_automatically && state.frame_timestamp > state.substitutions.last_loaded_timestamp + state.substitutions.load_delay_seconds)) {
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
                    BA_ERROR("Something went wrong while parsing the substitutions response");
                    state.substitutions.api_response.emplace("Something went wrong while parsing the response");
                } else {
                    state.substitutions.just_reloaded = true;
                    state.substitutions.last_loaded_timestamp = state.frame_timestamp;
                    std::tm time_info;
                    localtime_s(&time_info, &state.frame_timestamp);
                    state.substitutions.last_loaded = "Last loaded: ";
                    state.substitutions.last_loaded += (time_info.tm_mday < 10 ? "0" : "") + std::to_string(time_info.tm_mday) + "." +
                        (time_info.tm_mon < 10 ? "0" : "") + std::to_string(time_info.tm_mon) + "." +
                        std::to_string(1900 + time_info.tm_year) + " at " +
                        (time_info.tm_hour < 10 ? "0" : "") + std::to_string(time_info.tm_hour) + ":" +
                        (time_info.tm_min < 10 ? "0" : "") + std::to_string(time_info.tm_min) + ":" +
                        (time_info.tm_sec < 10 ? "0" : "") + std::to_string(time_info.tm_sec);
                }
            } else {
                BA_ERROR("Something went wrong while getting the response: {}", res->body);
                state.substitutions.api_response.emplace("Something went wrong while getting the response: " + res->body);
            }
        }
        else {
            auto err = res.error();
            BA_ERROR("HTTP error: {}", httplib::to_string(err));
            state.substitutions.api_response.emplace("HTTP error: " + httplib::to_string(err));
        }
    }
    ImGui::SameLine();
    ImGui::Text(state.substitutions.last_loaded.c_str());
    if (state.substitutions.api_response.has_value()) {
        ImGui::TextWrapped(state.login.api_response.value().c_str());
    }
    ImGui::End();
}