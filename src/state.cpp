#include "state.h"

State::State()
	: show_another_window(false), show_demo_window(true), clear_color(ImVec4(0.45f, 0.55f, 0.60f, 1.00f)) {
	fps_clamp = 0.0;
	auth = AuthState::AuthState();
	login = LoginState::LoginState();
}

State::~State() {
}