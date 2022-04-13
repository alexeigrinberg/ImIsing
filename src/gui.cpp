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

struct Dataplot {
    std::vector<double> data;
    const char* name;
    float vmin;
    float vmax;
    
    Dataplot (const char* n)
    {
        name = n;
    }

    Dataplot (const char* n, float vd, float vu)
    {
        name = n;
        vmin = vd;
        vmax = vu;
    }

};

void UpdateHLines(std::vector<double>& t_hline, int t, std::vector<double>& e_hline, double e, std::vector<double>& m_hline, double m)
{
    if (t==0)
    {
        e_hline[0]=e;
        m_hline[0]=m;
    }
    else if (t==1)
    {
        e_hline[0]=e;
        e_hline.push_back(e);
        
        m_hline[0]=m;
        m_hline.push_back(m);
        
        t_hline.push_back(1.0);
    }
    else
    {
        e_hline[0]=e;
        e_hline[1]=e;

        m_hline[0]=m;
        m_hline[1]=m;

        t_hline[1]=(double)t;
    }
}

void ResetHLines(std::vector<double>& t_hline, std::vector<double>& e_hline, std::vector<double>& m_hline)
{
    t_hline.clear();
    t_hline.push_back(0.0);

    e_hline.clear();
    e_hline.push_back(0.0);

    m_hline.clear();
    m_hline.push_back(0.0);
}

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

    // GUI Color
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    
    // List of beta values for collecting experimental data
    std::vector<double> test_betas{0.1,0.15,0.20,0.25,0.30,0.35,0.40,0.41,0.42,
        0.43, 0.44, 0.45, 0.46, 0.47, 0.48, 0.49, 0.50, 0.55, 0.60, 0.65, 0.70,
        0.75, 0.80, 0.85, 0.90, 0.95, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};
    
    // Independent variables 
    Dataplot sweep = Dataplot("Sweep");
    Dataplot temp = Dataplot("Temperature",0.0,10.5);
    
    // Energy variables
    Dataplot energy = Dataplot("Energy");
    Dataplot avg_energy = Dataplot("<Energy>",-2.1,0.0);

    std::vector<double> var_energy;
    double current_avg_energy = 0.0; //curent sample mean
    double current_M2_energy = 0.0; // sum of squares of differences from the current mean

    // Magnetization variables
    Dataplot mag = Dataplot("Magnetization");
    Dataplot avg_mag = Dataplot("<Magnetization>",0.0,1.0);

    std::vector<double> var_mag;
    double current_avg_mag = 0.0;
    double current_M2_mag = 0.0;

    // Vectors for plotting avg value as horizontal line
    std::vector<double> sweep_hline(1,0.0);
    std::vector<double> avg_energy_hline(1,0.0);
    std::vector<double> avg_mag_hline(1,0.0);

    // Specific Heat
    Dataplot cv = Dataplot("Specific Heat",0.0,0.0);

    // List of dependent variables for plotA and plotB
    Dataplot* plotA_options[2] = { &energy, &mag};
    std::vector<double>* plotA_hlines[2] = { &avg_energy_hline, &avg_mag_hline };
    Dataplot* plotB_options[3] = { &avg_energy, &avg_mag, &cv};

    // Which variable is currently being plotted
    Dataplot* current_plotA = &energy;
    std::vector<double>* current_hline = &avg_energy_hline;
    Dataplot* current_plotB = &avg_energy;

    // Experimental parameters and progress tracking
    int current_sweep = 0;
    int current_beta_idx = 0;
    int sweeps_per_point = 100;
    float progress = 0.0;

    // Slider bounds
    const double beta_low = 0.0971;
    const double beta_high = 2.0;
    const double field_low = -1.0;
    const double field_high = 1.0;

    // Graphing parameters
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
        
        // Widget sizes
        float pad = ImGui::GetStyle().FramePadding.x;
        float inner_pad = ImGui::GetStyle().ItemInnerSpacing.x;
        float checkbox_width = ImGui::GetFrameHeight();
        
        // Hacky way to get the width of strings of various lengths
        float three_char_width = ImGui::CalcTextSize("123").x;
        float four_char_width = ImGui::CalcTextSize("1234").x;
        float five_char_width = ImGui::CalcTextSize("12345").x;
        float six_char_width = ImGui::CalcTextSize("123456").x;
        float seven_char_width = ImGui::CalcTextSize("1234567").x;
        float eight_char_width = ImGui::CalcTextSize("12345678").x;
        float nine_char_width = ImGui::CalcTextSize("123456789").x;
        float ten_char_width = ImGui::CalcTextSize("1234567890").x;
        
        // Align the slider width with the row of buttons above it
        float slider_width = 4*ten_char_width + six_char_width + 5*checkbox_width + 26*pad;

        // combo box widths
        float size_combo_width = three_char_width + 2*pad + checkbox_width; //3 digits
        float plotA_combo_width = ten_char_width + three_char_width + 2*pad + checkbox_width; //13 char
        float plotB_combo_width = ten_char_width + five_char_width + 2*pad + checkbox_width; //15 char

        // progress bar is calculated relative to other widgets
        float plot_width = 600.0f;
        float collect_button_width = seven_char_width + 2.0f*pad;
        float reset_button_width = five_char_width + 2.0f*pad;
        float sweeps_width = six_char_width + 2.0f*pad;
        ImVec2 progress_bar_size = ImVec2(plot_width-collect_button_width-reset_button_width-2*sweeps_width-5*pad,0.0f);
        
        // Free play mode: constantly update the simulation    
        if (play)
        {
            // Only save up to max_data_size (5000) data points
            if (sweep.data.size() < max_data_size)
            {
                energy.data.push_back(sim->energy);
                mag.data.push_back(abs(sim->mag));
                sweep.data.push_back((double)current_sweep);
                
                // Welford's online algorithm for statistics 
                if (current_sweep==0)
                {
                    current_avg_energy += energy.data.back();
                    current_avg_mag += mag.data.back();
                }
                else
                {
                    double energy_delta = energy.data.back() - current_avg_energy;
                    double mag_delta = mag.data.back() - current_avg_mag;

                    current_avg_energy += energy_delta / (current_sweep+1);
                    current_avg_mag += mag_delta / (current_sweep+1);

                    double energy_delta2 = energy.data.back() - current_avg_energy;
                    double mag_delta2 = mag.data.back() - current_avg_mag;

                    current_M2_energy += energy_delta * energy_delta2;
                    current_M2_mag += mag_delta * mag_delta2;
                }
                // End Welford

                UpdateHLines(
                    sweep_hline,current_sweep,
                    avg_energy_hline,current_avg_energy,
                    avg_mag_hline,current_avg_mag
                ); 
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

        // Data collection mode: run a fixed set of beta values
        if (collecting)
        {
            if (current_beta_idx < test_betas.size())
            {
                sim->beta=test_betas[current_beta_idx];

                if (current_sweep<sweeps_per_point)
                {
                    energy.data.push_back(sim->energy);
                    mag.data.push_back(abs(sim->mag));
                    sweep.data.push_back((double)current_sweep);

                    // Welford's online algorithm for statistics 
                    if (current_sweep==0)
                    {
                        current_avg_energy += energy.data.back();
                        current_avg_mag += mag.data.back();
                    }
                    else
                    {
                        double energy_delta = energy.data.back() - current_avg_energy;
                        double mag_delta = mag.data.back() - current_avg_mag;

                        current_avg_energy += energy_delta / (current_sweep+1);
                        current_avg_mag += mag_delta / (current_sweep+1);

                        double energy_delta2 = energy.data.back() - current_avg_energy;
                        double mag_delta2 = mag.data.back() - current_avg_mag;

                        current_M2_energy += energy_delta * energy_delta2;
                        current_M2_mag += mag_delta * mag_delta2;
                    }
                    // End Welford
                    

                    UpdateHLines(
                        sweep_hline,current_sweep,
                        avg_energy_hline,current_avg_energy,
                        avg_mag_hline,current_avg_mag
                    );
                    
                    for (int i=0;i<sim->GetHeight()*sim->GetWidth();i++)
                    {
                        sim->UpdateMetropolis();
                    }
                    current_sweep++;
                }
                else
                {
                    temp.data.push_back(1/(sim->beta));

                    avg_energy.data.push_back(current_avg_energy);
                    var_energy.push_back(current_M2_energy/current_sweep);
                    current_avg_energy = 0.0;
                    current_M2_energy = 0.0;
                    
                    avg_mag.data.push_back(current_avg_mag);
                    var_mag.push_back(current_M2_mag/current_sweep);
                    current_avg_mag = 0.0;
                    current_M2_mag = 0.0;
                    ResetHLines(sweep_hline, avg_energy_hline, avg_mag_hline);
                    
                    energy.data.clear();
                    mag.data.clear();
                    sweep.data.clear();

                    current_sweep=0;
                    hot_start?sim->HotStart():sim->ColdStart();
                    
                    current_beta_idx++;
                }
                progress=(1.0f*current_beta_idx*sweeps_per_point + current_sweep)/(test_betas.size()*sweeps_per_point);

                UpdateImageFromSim(sim,image);
                UpdateTexture(image);
            }
            else
            {
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
            const char* start_name = play?"Pause##PauseSimulation":"Start"; 
            const char* collecting_name = collecting?" Pause ##PauseExperiment":(first_press?"Collect":"Resume ");
            
            ImGui::Begin("Ising Simulation");
            ImGui::BeginGroup();
            ImGui::Image((void*)(intptr_t)image_texture, ImVec2(image_width, image_height));
            
            ImGui::BeginDisabled(collecting || (!first_press)); 
            // Toggle between start and pause
            if (ImGui::Button(start_name))
            {
                play = 1 - play;
            }

            // Manually do one sweep (only if simulation is paused)
            ImGui::SameLine();
            ImGui::BeginDisabled(play);
            if (ImGui::Button("Step"))
            {
                // Only save up to max_data_size (5000) data points
                if (sweep.data.size() < max_data_size)
                {
                    energy.data.push_back(sim->energy);
                    mag.data.push_back(abs(sim->mag));
                    sweep.data.push_back((double)current_sweep);
                   
                    // Welford's online algorithm for statistics 
                    if (current_sweep==0)
                    {
                        current_avg_energy += energy.data.back();
                        current_avg_mag += mag.data.back();
                    }
                    else
                    {
                        double energy_delta = energy.data.back() - current_avg_energy;
                        double mag_delta = mag.data.back() - current_avg_mag;

                        current_avg_energy += energy_delta / (current_sweep+1);
                        current_avg_mag += mag_delta / (current_sweep+1);

                        double energy_delta2 = energy.data.back() - current_avg_energy;
                        double mag_delta2 = mag.data.back() - current_avg_mag;

                        current_M2_energy += energy_delta * energy_delta2;
                        current_M2_mag += mag_delta * mag_delta2;
                    }

                    UpdateHLines(
                        sweep_hline,current_sweep,
                        avg_energy_hline,current_avg_energy,
                        avg_mag_hline,current_avg_mag
                    );
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
            ImGui::EndDisabled();
            
            // Reset simulation to desired initial state (hot or cold)
            ImGui::SameLine();
            if (ImGui::Button("Reset##ResetSimulation"))
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
            
            ImGui::SetNextItemWidth(size_combo_width);
            if (ImGui::BeginCombo("Size",sizes_preview_value))
            {
                for (int n=0; n < IM_ARRAYSIZE(sizes); n++)
                {
                    const bool is_selected = (size_current_idx == n);
                    if (ImGui::Selectable(sizes[n], is_selected))
                    {
                        size_current_idx = n;
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }

                        double current_beta = sim->beta;
                        double current_field = sim->field;
                        int new_size = std::atoi(sizes[n]);
                        
                        // Make a new simulation with the same temperature and field paramters but a new size
                        delete[] sim;
                        Ising* sim = new Ising(new_size,new_size,current_beta,current_field);
                        hot_start?sim->HotStart():sim->ColdStart();
                        
                        // Update display
                        UpdateImageFromSim(sim, image);
                        UpdateTexture(image);
                    }
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
            
            //ImGui::PushItemWidth(34.f * ImGui::GetFontSize());
            ImGui::PushItemWidth(slider_width);
            ImGui::SliderScalar("Beta",ImGuiDataType_Double, &sim->beta, &beta_low, &beta_high,"%.4f",ImGuiSliderFlags_Logarithmic);
            if(ImGui::SliderScalar("Field",ImGuiDataType_Double, &sim->field, &field_low, &field_high,"%.2f"))
            {
                sim->energy = sim->CalcEnergy();
            }

            ImGui::PopItemWidth();
            ImGui::EndDisabled();
            
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
            if (ImPlot::BeginPlot("Simulation Monitor",ImVec2(plot_width,300)))
            {
                ImPlot::SetupAxes(sweep.name,current_plotA->name,ImPlotAxisFlags_AutoFit,ImPlotAxisFlags_AutoFit);
                ImPlot::PlotLine("Data",sweep.data.data(),current_plotA->data.data(),sweep.data.size());
                ImPlot::PlotLine("Avg",sweep_hline.data(),current_hline->data(),sweep_hline.size());
                ImPlot::EndPlot();
            }

            if (need_to_reset_graph)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0 / 7.0f, 0.6f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0 / 7.0f, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0 / 7.0f, 0.8f, 0.8f));
                
                ImGui::BeginDisabled(collecting || (!first_press));
                if (ImGui::Button("Reset Plot##ResetPlotA"))
                {
                    energy.data.clear();
                    mag.data.clear();
                    sweep.data.clear();
                    current_sweep=0;
                    
                    ResetHLines(sweep_hline, avg_energy_hline, avg_mag_hline);
                    current_avg_energy = 0.0;
                    current_M2_energy = 0.0;
                    current_avg_mag = 0.0;
                    current_M2_mag = 0.0;

                    need_to_reset_graph = false;
                }
                ImGui::PopStyleColor(3);
                ImGui::EndDisabled();
            }
            else
            {
                ImGui::BeginDisabled(collecting || (!first_press));
                if (ImGui::Button("Reset Plot##ResetPlotA"))
                {
                    energy.data.clear();
                    mag.data.clear();
                    sweep.data.clear();
                    current_sweep=0;
                    
                    ResetHLines(sweep_hline, avg_energy_hline, avg_mag_hline);
                    current_avg_energy = 0.0;
                    current_M2_energy = 0.0;
                    current_avg_mag = 0.0;
                    current_M2_mag = 0.0;
                }
                ImGui::EndDisabled();
            }
            
            // Plot Combo box
            ImGui::SameLine();
            
            static int plotA_current_idx = 0;
            const char* plotA_preview_value = plotA_options[plotA_current_idx]->name;
            
            ImGui::SetNextItemWidth(plotA_combo_width);
            if (ImGui::BeginCombo("Plot##PlotACombo",plotA_preview_value))
            {
                for (int n=0; n < IM_ARRAYSIZE(plotA_options); n++)
                {
                    const bool is_selected = (plotA_current_idx == n);
                    if (ImGui::Selectable(plotA_options[n]->name, is_selected))
                    {
                        plotA_current_idx = n;
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();

                        }
                        current_plotA = plotA_options[n];
                        current_hline = plotA_hlines[n];
                    }
                }
                ImGui::EndCombo();
            }


            // Data collecting plotting
            if (ImPlot::BeginPlot("Experiment Results",ImVec2(600,300)))
            {
                
                ImPlot::SetupAxesLimits(temp.vmin,temp.vmax,current_plotB->vmin,current_plotB->vmax,ImGuiCond_Always);
                ImPlot::SetupAxes(temp.name,current_plotB->name,0,0);
                ImPlot::PlotLine("Data",temp.data.data(),current_plotB->data.data(),temp.data.size());
                ImPlot::EndPlot();
            }

            ImGui::BeginDisabled(play);
            if (ImGui::Button(collecting_name))
            {
                if(first_press)
                {
                    energy.data.clear();
                    mag.data.clear();
                    sweep.data.clear();
                    current_sweep = 0;
                    current_beta_idx=0;
                   
                    ResetHLines(sweep_hline, avg_energy_hline, avg_mag_hline);
                    current_avg_energy = 0.0;
                    current_M2_energy = 0.0;
                    current_avg_mag = 0.0;
                    current_M2_mag = 0.0;

                    hot_start?sim->HotStart():sim->ColdStart();
                    sim->beta = test_betas[current_beta_idx];

                    first_press=false;
                    progress=0.0;
                }
                collecting = 1 - collecting;
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset##ResetExperiment"))
            {
                energy.data.clear();
                mag.data.clear();
                sweep.data.clear();
                current_sweep = 0;
                
                hot_start?sim->HotStart():sim->ColdStart();
                current_beta_idx = 0; 
                sim->beta = test_betas[current_beta_idx];
                UpdateImageFromSim(sim, image);
                UpdateTexture(image);
                
                current_avg_energy = 0.0;
                current_M2_energy = 0.0;
                current_avg_mag = 0.0;
                current_M2_mag = 0.0;


                ResetHLines(sweep_hline, avg_energy_hline, avg_mag_hline);
                
                avg_energy.data.clear();
                avg_mag.data.clear();
                temp.data.clear();

                collecting=false;
                first_press=true;
                progress=0.0;
            } 
            

            // Select sweeps per data point
            ImGui::SameLine();
            ImGui::BeginDisabled(collecting);
            ImGui::SetNextItemWidth(sweeps_width);
            ImGui::InputInt("Sweeps",&sweeps_per_point,0);
            ImGui::EndDisabled();
           
            // Experiment progress
            ImGui::SameLine();
            ImGui::ProgressBar(progress, progress_bar_size);
            ImGui::EndDisabled(); 
            
            if (ImGui::Button("Reset Plot##ResetPlotB"))
            {
                avg_energy.data.clear();
                avg_mag.data.clear();
                temp.data.clear();
            }
            
            // PlotB Combo box
            ImGui::SameLine();
            
            static int plotB_current_idx = 0;
            const char* plotB_preview_value = plotB_options[plotB_current_idx]->name;
            
            ImGui::SetNextItemWidth(plotB_combo_width);
            if (ImGui::BeginCombo("Plot##PlotBCombo",plotB_preview_value))
            {
                for (int n=0; n < IM_ARRAYSIZE(plotB_options); n++)
                {
                    const bool is_selected = (plotB_current_idx == n);
                    if (ImGui::Selectable(plotB_options[n]->name, is_selected))
                    {
                        plotB_current_idx = n;
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();

                        }
                        current_plotB = plotB_options[n];
                    }
                }
                ImGui::EndCombo();
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
