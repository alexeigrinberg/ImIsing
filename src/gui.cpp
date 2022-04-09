#include "imgui.h"
#include "implot.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "helper.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif


// Main code
int main(int, char**)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Ising Model Simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 1000, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    // Create simulation
    int sim_width = 400;
    int sim_height = 400;
    Ising* sim = new Ising(sim_width, sim_height);
    sim->HotStart();

    // Initialize texture
    GLuint image_texture = 0;
    bool ret = SetupTexture(&image_texture);    
    IM_ASSERT(ret); 
    
    // Create image and update texture
    int image_width = 800;
    int image_height = 800;
    LatticeImage* image = new LatticeImage(image_width, image_height);
    UpdateImageFromSim(sim,image);
    UpdateTexture(image);
    
    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool play = false;
    bool collecting = false;
    bool show_demo_window = false;
    bool show_plot_window = false;
    bool hot_start = true;
    bool wolff = false;
    bool state_toggle = true;
    bool first_press = true;
    bool need_to_reset_graph = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    std::vector<double> energies; 
    std::vector<double> mags;
    std::vector<double> times;

    std::vector<double> avg_energies;
    std::vector<double> temps;
    std::vector<double> test_betas{0.1,0.15,0.20,0.25,0.30,0.35,0.40,0.41,0.42,
        0.43, 0.44, 0.45, 0.46, 0.47, 0.48, 0.49, 0.50, 0.55, 0.60, 0.65, 0.70,
        0.75, 0.80, 0.85, 0.90, 0.95, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};

    int current_sweep = 0;
    int current_beta_idx = 0;

    // Slider bounds
    const double beta_low = 0.0971;
    const double beta_high = 2.0;
    const double field_low = -1.0;
    const double field_high = 1.0;

    // graphing parameters
    int max_data_size = 5000;

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        
        if (play)
        {
            // Only save up to max_data_size (5000) data points
            if (times.size() < max_data_size)
            {
                energies.push_back(sim->energy);
                mags.push_back(abs(sim->mag));
                times.push_back((double)current_sweep);
            }
            else
            {
                need_to_reset_graph = true;
            }

            for (int i=0; i<sim->GetHeight()*sim->GetWidth(); i++)
            {
                sim->UpdateMetropolis();
            }
            current_sweep++;
            UpdateImageFromSim(sim,image);
            UpdateTexture(image);
        }

        if (collecting)
        {
            if (current_beta_idx < test_betas.size())
            {
                sim->beta=test_betas[current_beta_idx];

                if (current_sweep<100)
                {
                    energies.push_back(sim->energy);
                    mags.push_back(abs(sim->mag));
                    times.push_back((double)current_sweep);
                    
                    for (int i=0;i<sim->GetHeight()*sim->GetWidth();i++)
                    {
                        sim->UpdateMetropolis();
                    }
                    current_sweep++;
                }
                else
                {
                    temps.push_back(1/(sim->beta));
                    avg_energies.push_back(sim->energy);
                    
                    energies.clear();
                    mags.clear();
                    times.clear();
                    current_sweep=0;
                    hot_start?sim->HotStart():sim->ColdStart();
                    
                    current_beta_idx++;
                }

                UpdateImageFromSim(sim,image);
                UpdateTexture(image);
            }
            else
            {
                current_beta_idx = 0;
                first_press = true;
                collecting = 1 - collecting;
            }
        }

        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        if (show_plot_window)
        {
            ImPlot::ShowDemoWindow(&show_plot_window);
        }
        
        // Main window
        {
            const char* start_name = play?"Pause":"Start"; 
            const char* collecting_name = collecting?"Pause":"Collect";
            
            ImGui::Begin("Ising Simulation");
            ImGui::BeginGroup();
            ImGui::Image((void*)(intptr_t)image_texture, ImVec2(image_width, image_height));
            
            
            
            // Toggle between start and pause
            if (ImGui::Button(start_name))
            {
                play = 1 - play;
            }

            // Manually do one sweep (only if simulation is paused)
            ImGui::SameLine();
            if (play)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
                ImGui::Button("Step");
                ImGui::PopStyleColor(3);            
            }
            else
            {
                if (ImGui::Button("Step"))
                {
                    // Only save up to max_data_size (5000) data points
                    if (times.size() < max_data_size)
                    {
                        energies.push_back(sim->energy);
                        mags.push_back(abs(sim->mag));
                        times.push_back((double)current_sweep);
                    }
                    else
                    {
                        need_to_reset_graph = true;
                    }
                    
                    for (int i=0; i<sim->GetHeight()*sim->GetWidth(); i++)
                    {
                        sim->UpdateMetropolis();
                    }
                    current_sweep++;
                    UpdateImageFromSim(sim, image);
                    UpdateTexture(image);
                }
            }
            
            // Reset simulation to desired initial state (hot or cold)
            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                hot_start?sim->HotStart():sim->ColdStart();
                UpdateImageFromSim(sim, image);
                UpdateTexture(image);
            }

            // Size Combo box
            ImGui::SameLine();
            const char* sizes[] = { "800", "400", "200", "100", "50", "25" };
            static int size_current_idx = 1;
            const char* sizes_preview_value = sizes[size_current_idx];
            ImGui::SetNextItemWidth(4.0f * ImGui::GetFontSize());
            if (ImGui::BeginCombo("Size",sizes_preview_value))
            {
                for (int n=0; n < IM_ARRAYSIZE(sizes); n++)
                {
                    const bool is_selected = (size_current_idx == n);
                    if (ImGui::Selectable(sizes[n], is_selected))
                    {
                        size_current_idx = n;
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();

                        double current_beta = sim->beta;
                        double current_field = sim->field;
                        int new_size = std::atoi(sizes[n]);
                        
                        // Make a new simulation with the same temperature and field paramters but a new size
                        delete[] sim;
                        Ising* sim = new Ising(new_size,new_size,current_beta,current_field);
                        hot_start?sim->HotStart():sim->ColdStart();
                        
                        // Update display
                        UpdateImageFromSim(sim, image);
                        UpdateTexture(image);}
                }
                ImGui::EndCombo();
            }
            
            // Spin-up color picker
            ImGui::SameLine();
            if (ImGui::ColorEdit4("Spin-up",image->color_a,ImGuiColorEditFlags_NoInputs))
            {
                UpdateImageFromSim(sim, image);
                UpdateTexture(image);
            }
            
            // Spin-down color picker
            ImGui::SameLine();
            if (ImGui::ColorEdit4("Spin-down",image->color_b,ImGuiColorEditFlags_NoInputs))
            {
                UpdateImageFromSim(sim, image);
                UpdateTexture(image);
            }

            
            // Initial spin configurations
            ImGui::SameLine();
            ImGui::Checkbox("Hot Start", &hot_start);

            ImGui::SameLine();
            ImGui::Checkbox("Wolff Cluster", &wolff);
            
            ImGui::PushItemWidth(34.f * ImGui::GetFontSize());
            ImGui::SliderScalar("Beta",ImGuiDataType_Double, &sim->beta, &beta_low, &beta_high,"%.4f",ImGuiSliderFlags_Logarithmic);
            if(ImGui::SliderScalar("Field",ImGuiDataType_Double, &sim->field, &field_low, &field_high,"%.2f"))
            {
                sim->energy = sim->CalcEnergy();
            }

            ImGui::PopItemWidth();

            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::SameLine();
            ImGui::Checkbox("Plot Demo Window", &show_plot_window);
            
            ImGui::SameLine();            
            ImGui::Checkbox("State toggle", &state_toggle);
            ImGui::SameLine();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::EndGroup();
            
            // Plotting
            ImGui::SameLine();
            ImGui::BeginGroup();
            if (ImPlot::BeginPlot("Energy",ImVec2(600,300)))
            {
                ImPlot::SetupAxes(NULL,NULL,ImPlotAxisFlags_AutoFit,ImPlotAxisFlags_AutoFit);
                ImPlot::PlotLine("Test",times.data(),energies.data(),times.size());
                ImPlot::EndPlot();
            }

            if (need_to_reset_graph)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
                if (ImGui::Button("Reset plot"))
                {
                    energies.clear();
                    mags.clear();
                    times.clear();
                    current_sweep=0;
                    need_to_reset_graph = false;
                }
                ImGui::PopStyleColor(3);
            }
            else
            {
                if (ImGui::Button("Reset plot"))
                {
                    energies.clear();
                    mags.clear();
                    times.clear();
                    current_sweep=0;
                }
            }

            if (ImPlot::BeginPlot("<Energy>",ImVec2(600,300)))
            {
                ImPlot::SetupAxesLimits(0,10.5,-2.1,0);
                ImPlot::SetupAxes(NULL,NULL,0,0);
                ImPlot::PlotScatter("Test",temps.data(),avg_energies.data(),temps.size());
                ImPlot::EndPlot();
            }
            if (ImGui::Button(collecting_name))
            {
                if(first_press)
                {
                    energies.clear();
                    mags.clear();
                    times.clear();
                    current_sweep = 0;
                    
                    hot_start?sim->HotStart():sim->ColdStart();
                    sim->beta = test_betas[current_beta_idx];

                    first_press=false;
                }
                collecting = 1 - collecting;
            }
            ImGui::EndGroup();
            
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    delete[] sim;
    delete[] image;

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
