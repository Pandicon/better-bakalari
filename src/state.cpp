#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <fstream>
#include <httplib.h>
#include <iostream>
#include <json/json.h>

#include "log.h"
#include "state.h"

State::State()
	: show_another_window(false), show_demo_window(true), clear_color(ImVec4(0.45f, 0.55f, 0.60f, 1.00f)) {
	fps_clamp = 0.0;
	api_url = std::nullopt;
	auth = AuthState::AuthState();
	login = LoginState::LoginState();
}

State::~State() {
}

void State::init() {
	show_demo_window = true;
	show_another_window = false;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	bool save_file_loaded = false;
	std::ifstream save_file("save.json");
	if (save_file.is_open()) {
		Json::Value save_data;
		Json::Reader reader;
		bool parsingSuccessful = reader.parse(save_file, save_data);
		if (parsingSuccessful) {
			if (save_data["api_url"].asString().length() > 0) {
				api_url.emplace(save_data["api_url"].asString());
				save_file_loaded = true;
			}
			BA_DEBUG("Save file loaded successfully");
		}
		save_file.close();
	}
	if (!save_file_loaded) {
		BA_ERROR("Failed to load save file");
		std::ofstream save_file_output("save.json");
		if (save_file_output.is_open()) {
			save_file_output << "{}";
			save_file_output.close();
		}
	}

	auth.init();

	if (api_url.has_value()) {
		login.api_url = api_url.value();
	}

	if (auth.refresh_token.has_value()) {
		get_access_token(auth.refresh_token.value());
	}
}

std::optional<std::string> State::get_access_token(std::string refresh_tok) {
	if (!api_url.has_value()) return std::nullopt;

	httplib::SSLClient ssl_client(api_url.value());
	ssl_client.set_ca_cert_path("./resources/ca-bundle.crt");
	ssl_client.set_connection_timeout(0, 300000); // 300 milliseconds
	ssl_client.set_read_timeout(5, 0); // 5 seconds
	ssl_client.set_write_timeout(5, 0); // 5 seconds

	auto res = ssl_client.Post("/api/login", "client_id=ANDR&grant_type=refresh_token&refresh_token=" + refresh_tok, "application/x-www-form-urlencoded");
	if (!res) {
		auto err = res.error();
		BA_ERROR("HTTP error when loading the access token: {}", httplib::to_string(err));
		return std::nullopt;
	}
	if (res->status != 200) {
		BA_ERROR("Got response code {} when loading the access token\nResponse body: {}", res->status, res->body);
		return std::nullopt;
	}
	Json::Value body;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(res->body, body);
	if (parsingSuccessful) {
		std::string access_token = body["access_token"].asString();
		std::string refresh_token = body["refresh_token"].asString();
		int expires_in = body["expires_in"].asInt();
		const auto p1 = std::chrono::system_clock::now();
		int curr_timestamp = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
		auth.access_token = access_token;
		auth.refresh_token = refresh_token;
		auth.access_token_expires_at = curr_timestamp + expires_in;

		std::ofstream refresh_token_save("./token.b64", std::ofstream::trunc);
		if (refresh_token_save.is_open()) {
			refresh_token_save << refresh_token;
			refresh_token_save.close();
		}

		login.api_response = std::nullopt;

		login.username = "";
		login.password = "";

		BA_DEBUG("Access token acquired successfully");

		return access_token;
	}
	return std::nullopt;
}