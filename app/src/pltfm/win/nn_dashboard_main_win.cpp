#include "imgui_include.hpp"
#include "../../../../libs/sdl/sdl_include.hpp"
#include "../../display/display.hpp"
#include "../../diagnostics/diagnostics.hpp"


static void set_game_window_icon(SDL_Window* window)
{
#include "../../../../resources/icon_64.c" // this will "paste" the struct my_icon into this function
    sdl::set_window_icon(window, icon_64);
}


static void texture_window(cstr title, void* texture, u32 width, u32 height, f32 scale)
{
    auto w = width * scale;
    auto h = height * scale;

    ImGui::Begin(title);

    ImGui::Image(texture, ImVec2(w, h));

    ImGui::End();
}


enum class RunState : int
{
    Begin,
    Run,
    End
};


namespace
{
    SDL_Window* window = 0;
    ui::UIState ui_state{};

    RunState run_state = RunState::Begin;

    constexpr u32 N_TEXTURES = 1;
    constexpr dx11::TextureId input_image_texture_id = { 0 };

    dx11::Context dx_ctx;    
    dx11::TextureList<N_TEXTURES> textures;

    display::DisplayState display_state;

#ifdef NDEBUG

    // Data assumed to be in current directory
    #define ROOT "./"

#else
    // your data path here
    // zip file available in the downloads folder
    #define ROOT "C:\\D_Data\\Repos\\NNDashboard/resources/test_data/"

#endif

    constexpr mlai::DataFiles ai_files = {
        ROOT "train-images.idx3-ubyte",
        ROOT "t10k-images.idx3-ubyte",
        ROOT "train-labels.idx1-ubyte",
        ROOT "t10k-labels.idx1-ubyte"
    };
}


static void end_program()
{
    run_state = RunState::End;
}


static bool is_running()
{
    return run_state != RunState::End;
}


static void handle_window_event(SDL_Event const& event, SDL_Window* window)
{
    auto const window_resize = [&]()
    {
        if (event.window.windowID == SDL_GetWindowID(window))
        {
            return;
        }

        // TODO
        int w, h;
        SDL_GetWindowSize(window, &w, &h);

        // Release all outstanding references to the swap chain's buffers before resizing.
        dx11::cleanup_render_target(dx_ctx);
        dx_ctx.pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
        dx11::create_render_target(dx_ctx);
    };

    auto const window_close = [&]()
    {
        if (event.window.windowID == SDL_GetWindowID(window))
        {
            end_program();
        }
    };

    switch (event.type)
    {
    case SDL_QUIT:
        end_program();
        break;

    case SDL_WINDOWEVENT:
    {
        switch (event.window.event)
        {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        case SDL_WINDOWEVENT_RESIZED:
            window_resize();
            break;

        case SDL_WINDOWEVENT_CLOSE:
            window_close();
            break;
        
        default:
            break;
        }
    } break;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        auto key_code = event.key.keysym.sym;
        switch (key_code)
        {
        case SDLK_ESCAPE:
            sdl::print_message("ESC");
            end_program();
            break;

        default:
            break;
        }

    } break;
    
    default:
        break;
    }
}


static void process_user_input()
{
    SDL_Event event;

    // Poll and handle events (inputs, window resize, etc.)
    while (SDL_PollEvent(&event))
    {
        handle_window_event(event, window);
        ImGui_ImplSDL2_ProcessEvent(&event);
    }
}


static void render_imgui_frame()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Rendering
    ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_None);

#ifdef SHOW_IMGUI_DEMO
    ui::show_imgui_demo(ui_state);
#endif

    display::show_display(display_state);
    diagnostics::show_diagnostics();

    ImGui::Render();

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    dx11::render(dx_ctx, clear_color);    
}


static bool ui_init()
{
    if(!sdl::init())
    {       
        sdl::print_message("Error: sdl::init()"); 
        return false;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // fullscreen window
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    int window_w = 1382;
    int window_h = 777;

    window = ui::create_sdl_dx11_window("Machine Learning", window_w, window_h);
    if (!window)
    {
        sdl::print_error("Error: create_sdl_ogl_window()");
        sdl::close();
        return false;
    }

    set_game_window_icon(window);

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    HWND hwnd = (HWND)wmInfo.info.win.window;

    // Initialize Direct3D
    if (!dx11::init_context(dx_ctx, hwnd))
    {
        dx11::close_context(dx_ctx);
        return false;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForD3D(window);
    ImGui_ImplDX11_Init(dx_ctx.pd3dDevice, dx_ctx.pd3dDeviceContext);

    ui::set_imgui_style();
    ui_state.io = &io;

    textures = dx11::create_textures<N_TEXTURES>();

    return true;
}


static bool main_init()
{
    if (!ui_init())
    {
        return false;
    }

    display_state.ai_files = ai_files;

    if (!display::init(display_state))
    {
        return false;
    }

    auto& src = display_state.input_image;

    dx11::init_texture(src.data_, src.width, src.height, textures.get_dx_texture(input_image_texture_id), dx_ctx);
    display_state.input_texture = textures.get_imgui_texture(input_image_texture_id);

    return true;
}


static void main_close()
{
    display::destroy(display_state);

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    dx11::close_context(dx_ctx);

    SDL_DestroyWindow(window);
    sdl::close();
}


static void main_loop()
{    
    while(is_running())
    {
        process_user_input();
        
        dx11::render_texture(textures.get_dx_texture(input_image_texture_id), dx_ctx);

        render_imgui_frame(); 
    }
}


int main()
{
    if (!main_init())
    {
        return EXIT_FAILURE;
    }

    run_state = RunState::Run;

    main_loop();

    main_close();

    return EXIT_SUCCESS;
}

#include "main_o.cpp"