#define GL_SILENCE_DEPRECATION

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "application.h"
#include "imgui_utils.h"
#include "log.h"

const char SPECIAL_CHARACTERS[58] = u8"ěščřžýáíéúůďťňĚŠČŘŽÝÁÍÉÚŮĎŤŇ";

Application::Application()
    : io(ImGui::GetIO()), window(nullptr) {
    Log::Init();
}

Application::~Application() {
}

/*
Initialise state variables, ImGUI settings, fonts, state...
*/
void Application::Init(GLFWwindow* window_in, const char* glsl_version) {
    state = State::State();
    window = window_in;

    io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddText(SPECIAL_CHARACTERS);                   // Add a string
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());  // Add one of the default ranges
    builder.BuildRanges(&ranges);                          // Build the final result (ordered ranges with all the unique characters submitted)
    io.Fonts->AddFontFromFileTTF("./resources/Roboto-Medium.ttf", 14.0f, nullptr, ranges.Data);
    io.Fonts->Build();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    state.init();
}

/*
Start the Dear ImGui frame
*/
void Application::NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

/*
ImGUI UI code
*/
void Application::Update() {
    state.frame_timestamp = std::time(nullptr);

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (state.show_demo_window)
        ImGui::ShowDemoWindow(&state.show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &state.show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &state.show_another_window);
        
        ImGui::InputFloat("FPS clamp", &state.fps_clamp);
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&state.clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button")) {                          // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
            if (counter % 3 == 0)
                ImGui::StyleColorsDark();
            if (counter % 3 == 1)
                ImGui::StyleColorsClassic();
            if (counter % 3 == 2)
                ImGui::StyleColorsLight();
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        HelpMarker("Display all characters as '*'.\nDisable clipboard cut and copy.\nDisable logging.\n");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (state.show_another_window)
    {
        ImGui::Begin("Another Window", &state.show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            state.show_another_window = false;
        ImGui::End();
    }

    if (!state.is_access_token_valid() && state.auth.refresh_token.has_value()) {
        state.get_access_token(state.auth.refresh_token.value());
    }
    // Show login window
    if (!state.auth.access_token.has_value() || (!state.is_access_token_valid() && !state.auth.refresh_token.has_value())) {
        render_login();
    }
    if (state.is_access_token_valid()) {
        if (state.substitutions.load_automatically && state.frame_timestamp > state.substitutions.last_loaded_timestamp + state.substitutions.load_delay_seconds) {
            update_substitutions();
        }
        render_substitutions();
    }
    if (state.show_settings_window) {
        render_settings();
    }
}

/*
Update OpenGL buffers and render ImGUI
*/
void Application::Render() {
    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(state.clear_color.x * state.clear_color.w, state.clear_color.y * state.clear_color.w, state.clear_color.z * state.clear_color.w, state.clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwSwapBuffers(window);
}

/*
Destroy the window, terminate GLFW and ImGUI
*/
void Application::Shutdown() {
    spdlog::shutdown();

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}