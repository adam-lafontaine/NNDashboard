#pragma once


#if defined(_WIN32)
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>

#ifndef NDEBUG
#define PRINT_MESSAGES
#endif

#ifdef __EMSCRIPTEN__
#define NO_CONTROLLER
#define NO_WINDOW
#endif

#ifdef PRINT_MESSAGES
#include <cstdio>
#endif


namespace sdl
{
    static void print_message(const char* msg)
    {
    #ifdef PRINT_MESSAGES
        printf("%s\n", msg);
    #endif
    }
}





namespace sdl
{
    //constexpr auto SDL_OPTIONS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC;

    constexpr auto SDL_OPTIONS = SDL_INIT_VIDEO;
}



namespace sdl
{
    static void print_error(const char* msg)
    {
#ifdef PRINT_MESSAGES
        printf("%s\n%s\n", msg, SDL_GetError());
#endif
    }


    static void close()
    {
        SDL_Quit();
    }


    static bool init()
    {    
        if (SDL_Init(SDL_OPTIONS) != 0)
        {
            print_error("SDL_Init failed");
            return false;
        }

        return true;
    }


    static void display_error(const char* msg)
    {
#ifndef NO_WINDOW
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "ERROR", msg, 0);
#endif

        print_error(msg);
    }


    static void handle_window_event(SDL_WindowEvent const& w_event)
    {
#ifndef NO_WINDOW

        auto window = SDL_GetWindowFromID(w_event.windowID);

        switch(w_event.event)
        {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            {

            }break;
            case SDL_WINDOWEVENT_EXPOSED:
            {
                
            } break;
        }

#endif
    }
    

    template <typename T>
    static void set_window_icon(SDL_Window* window, T const& icon_64)
    {
        // these masks are needed to tell SDL_CreateRGBSurface(From)
        // to assume the data it gets is byte-wise RGB(A) data
        Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        int shift = (icon_64.bytes_per_pixel == 3) ? 8 : 0;
        rmask = 0xff000000 >> shift;
        gmask = 0x00ff0000 >> shift;
        bmask = 0x0000ff00 >> shift;
        amask = 0x000000ff >> shift;
#else // little endian, like x86
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = (icon_64.bytes_per_pixel == 3) ? 0 : 0xff000000;
#endif

        SDL_Surface* icon = SDL_CreateRGBSurfaceFrom(
            (void*)icon_64.pixel_data,
            icon_64.width,
            icon_64.height,
            icon_64.bytes_per_pixel * 8,
            icon_64.bytes_per_pixel * icon_64.width,
            rmask, gmask, bmask, amask);

        SDL_SetWindowIcon(window, icon);

        SDL_FreeSurface(icon);
    }


    static void toggle_fullscreen(SDL_Window* window)
    {
        static bool is_fullscreen = false;

        if (is_fullscreen)
        {
            SDL_SetWindowFullscreen(window, 0);
        }
        else
        {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        }

        is_fullscreen = !is_fullscreen;
    }
}

