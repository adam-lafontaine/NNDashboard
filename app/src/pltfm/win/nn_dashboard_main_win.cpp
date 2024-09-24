#include "imgui_include.hpp"
#include "../../../../libs/sdl/sdl_include.hpp"


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

    RunState run_state = RunState::Begin;
}


static void end_program()
{
    run_state = RunState::End;
}


static bool is_running()
{
    return run_state != RunState::End;
}


int main()
{
    return EXIT_SUCCESS;
}