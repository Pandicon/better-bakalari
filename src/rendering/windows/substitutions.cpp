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
                auto parsed_substitutions = state.substitutions.parse_from_json_string(res->body);
                if(!parsed_substitutions.has_value()) {
                    BA_ERROR("Something went wrong while parsing the substitutions response");
                    state.substitutions.api_response.emplace("Something went wrong while parsing the response");
                } else {
                    state.substitutions.substitution_days = parsed_substitutions.value();
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
                    std::optional<std::vector<SubstitutionDay>> parsed_past_substitutions = std::nullopt;
                    std::ifstream substitutions_log_in("./substitutions_log.json");
                    if (substitutions_log_in.is_open()) {
                        std::stringstream buffer;
                        buffer << substitutions_log_in.rdbuf();
                        parsed_past_substitutions = state.substitutions.parse_from_json_string(buffer.str());
                        substitutions_log_in.close();
                    }
                    std::ofstream substitutions_log_out("./substitutions_log.json", std::ofstream::trunc);
                    if (substitutions_log_out.is_open()) {
                        substitutions_log_out << res->body;
                        substitutions_log_out.close();
                    }
                    std::vector<SubstitutionDay> current_substitutions = parsed_substitutions.value();
                    std::vector<SubstitutionDay> past_substitutions = parsed_past_substitutions.has_value() ? parsed_past_substitutions.value() : std::vector<SubstitutionDay>();
                    // Erase substitutions that are both present in the current data and the saved data = unchanged substitutions
                    int cp = 0, pp = 0;
                    while (cp < current_substitutions.size() && pp < past_substitutions.size()) {
                        if (current_substitutions[cp].day_raw != past_substitutions[pp].day_raw) {
                            if (current_substitutions[cp].day_raw < past_substitutions[pp].day_raw) {
                                cp += 1;
                            } else {
                                pp += 1;
                            }
                            continue;
                        }
                        int cdp = 0;
                        while (cdp < current_substitutions[cp].substitutions.size()) {
                            auto curr_subs = current_substitutions[cp].substitutions[cdp];
                            auto it = std::find_if(past_substitutions[pp].substitutions.begin(), past_substitutions[pp].substitutions.end(), [&curr_subs](const Substitution& past_subs) {
                                return curr_subs.day == past_subs.day && curr_subs.hours == past_subs.hours && curr_subs.time == past_subs.time && curr_subs.subject == past_subs.subject && curr_subs.description == past_subs.description && curr_subs.change_type == past_subs.change_type;
                                });
                            if (it != past_substitutions[pp].substitutions.end()) {
                                auto index = std::distance(past_substitutions[pp].substitutions.begin(), it);
                                past_substitutions[pp].substitutions.erase(past_substitutions[pp].substitutions.begin() + index);
                                current_substitutions[cp].substitutions.erase(current_substitutions[cp].substitutions.begin() + cdp);
                            } else {
                                cdp += 1;
                            }
                        }
                        cp += 1;
                        pp += 1;
                    }
                    // Erase empty days = days without changed substitutions
                    cp = pp = 0;
                    while (cp < current_substitutions.size()) {
                        if (current_substitutions[cp].substitutions.empty()) {
                            current_substitutions.erase(current_substitutions.begin() + cp);
                        } else {
                            cp += 1;
                        }
                    }
                    while (pp < past_substitutions.size()) {
                        if (past_substitutions[pp].substitutions.empty()) {
                            past_substitutions.erase(past_substitutions.begin() + pp);
                        } else {
                            pp += 1;
                        }
                    }
                    // Go through past and current substitutions in order and log the changes
                    // If there is a substitution in the log but not in the current data, it was removed
                    // If there is a substitution in the log and in the current data, there was a change
                    // If there is a substitution in the current data but not in the lot, it was added
                    if (!current_substitutions.empty() || !past_substitutions.empty()) {
                        BA_INFO("--- SUBSTITUTIONS CHANGED ---");
                    }
                    cp = pp = 0;
                    while (cp < current_substitutions.size() || pp < past_substitutions.size()) {
                        if (cp < current_substitutions.size() && pp < past_substitutions.size()) {
                            if (current_substitutions[cp].day_raw < past_substitutions[pp].day_raw) {
                                // The current day is before the currently checked day from the logs -> there can be no changes, everything was added
                                for (auto substitution : current_substitutions[cp].substitutions) {
                                    BA_INFO("Substitution added: {} | {} | {}, {}", current_substitutions[cp].day, substitution.hours, substitution.time, substitution.description);
                                }
                                cp += 1;
                            } else if (current_substitutions[cp].day_raw > past_substitutions[pp].day_raw) {
                                // The currently checked day from the logs is before the current day -> there can be no changes, everything was removed
                                for (auto substitution : past_substitutions[pp].substitutions) {
                                    BA_INFO("Substitution removed: {} | {} | {}, {}", past_substitutions[pp].day, substitution.hours, substitution.time, substitution.description);
                                }
                                pp += 1;
                            } else {
                                int cdp = 0;
                                while (cdp < current_substitutions[cp].substitutions.size()) {
                                    auto curr_subs = current_substitutions[cp].substitutions[cdp];
                                    auto it = std::find_if(past_substitutions[pp].substitutions.begin(), past_substitutions[pp].substitutions.end(), [&curr_subs](const Substitution& past_subs) {
                                        return curr_subs.hours == past_subs.hours && curr_subs.time == past_subs.time;
                                        });
                                    if (it != past_substitutions[pp].substitutions.end()) {
                                        auto index = std::distance(past_substitutions[pp].substitutions.begin(), it);
                                        BA_INFO("Substitution changed: {} | {} | {} from {} to {}", current_substitutions[cp].day, curr_subs.hours, curr_subs.time, past_substitutions[pp].substitutions[index].description, curr_subs.description);
                                        past_substitutions[pp].substitutions.erase(past_substitutions[pp].substitutions.begin() + index);
                                        current_substitutions[cp].substitutions.erase(current_substitutions[cp].substitutions.begin() + cdp);
                                    } else {
                                        cdp += 1;
                                    }
                                }
                                // Now there are no substitution pairs from the same time -> only additions and deletions
                                for (auto substitution : current_substitutions[cp].substitutions) {
                                    BA_INFO("Substitution added: {} | {} | {}, {}", current_substitutions[cp].day, substitution.hours, substitution.time, substitution.description);
                                }
                                for (auto substitution : past_substitutions[pp].substitutions) {
                                    BA_INFO("Substitution removed: {} | {} | {}, {}", past_substitutions[pp].day, substitution.hours, substitution.time, substitution.description);
                                }
                                cp += 1;
                                pp += 1;
                            }
                        } else if (cp < current_substitutions.size()) {
                            // All the substitutions this day were added
                            for (auto substitution : current_substitutions[cp].substitutions) {
                                BA_INFO("Substitution added: {} | {} | {}, {}", current_substitutions[cp].day, substitution.hours, substitution.time, substitution.description);
                            }
                            cp += 1;
                        } else if (pp < past_substitutions.size()) {
                            // All the substitutions this day were removed
                            for (auto substitution : past_substitutions[pp].substitutions) {
                                BA_INFO("Substitution removed: {} | {} | {}, {}", past_substitutions[pp].day, substitution.hours, substitution.time, substitution.description);
                            }
                            pp += 1;
                        }
                    }
                }
            } else {
                BA_ERROR("Something went wrong while getting the response: {}", res->body);
                state.substitutions.api_response.emplace("Something went wrong while getting the response: " + res->body);
                if (res->status == 401) { // Invalid access token
                    state.auth.access_token = std::nullopt;
                }
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
        ImGui::TextWrapped(state.substitutions.api_response.value().c_str());
    }
    ImGui::End();
}