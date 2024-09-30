#pragma once

#include "imgui_options.hpp"

#include "../../../../libs/imgui/imgui.h"

#include "../../../../libs/imgui/backends/imgui_impl_sdl2.h"
#include "../../../../libs/imgui/backends/imgui_impl_dx11.h"

#include <d3d11.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>


namespace ui
{
    class UIState
    {
    public:

#ifdef SHOW_IMGUI_DEMO

        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#endif

        ImGuiIO* io = nullptr;
    };

}


/* demo window */

namespace ui
{

#ifdef SHOW_IMGUI_DEMO

    static inline void show_imgui_demo(UIState& state)
    {
        auto& io = *state.io;

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

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&state.clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

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
    }

#endif

}


namespace ui
{
    static inline SDL_Window* create_sdl_dx11_window(const char* title, int width, int height)
    {
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
        SDL_Window* window = SDL_CreateWindow(
            title, 
            SDL_WINDOWPOS_CENTERED, 
            SDL_WINDOWPOS_CENTERED, 
            width,
            height,
            window_flags);
        
        return window;
    }


    static inline void set_imgui_style()
    {
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
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != nullptr);

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        constexpr auto TEXT_WHITE = ImVec4(0.7f, 0.7f, 0.7f, 1);

        auto& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_Text] = TEXT_WHITE;
        style.TabRounding = 0.0f;
    }
}


namespace dx11
{
    class Context
    {
    public:
        ID3D11Device*            pd3dDevice = nullptr;
        ID3D11DeviceContext*     pd3dDeviceContext = nullptr;
        IDXGISwapChain*          pSwapChain = nullptr;
        ID3D11RenderTargetView*  mainRenderTargetView = nullptr;
    };


    static void create_render_target(Context& ctx)
    {
        ID3D11Texture2D* pBackBuffer;
        ctx.pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        ctx.pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &ctx.mainRenderTargetView);
        pBackBuffer->Release();
    }


    static void cleanup_render_target(Context& ctx)
    {
        if (ctx.mainRenderTargetView) { ctx.mainRenderTargetView->Release(); ctx.mainRenderTargetView = nullptr; }
    }


    static bool init_context(Context& ctx, HWND hWnd)
    {
        // Setup swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
        //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

        auto res = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 
            createDeviceFlags, featureLevelArray, 2, 
            D3D11_SDK_VERSION, &sd, 
            &ctx.pSwapChain, 
            &ctx.pd3dDevice, 
            &featureLevel, 
            &ctx.pd3dDeviceContext
            );
        
        if (res != S_OK)
        {
            return false;
        }            

        create_render_target(ctx);
        return true;
    }


    static void close_context(Context& ctx)
    {
        auto const release = [](auto& p) 
        {
            if (p)
            {
                p->Release();
                p = nullptr;
            }
        };

        release(ctx.mainRenderTargetView);
        release(ctx.pSwapChain);
        release(ctx.pd3dDeviceContext);
        release(ctx.pd3dDevice);
    }


    static void render(Context& ctx, ImVec4 clear_color)
    {
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        
        ctx.pd3dDeviceContext->OMSetRenderTargets(1, &ctx.mainRenderTargetView, nullptr);
        ctx.pd3dDeviceContext->ClearRenderTargetView(ctx.mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        ctx.pSwapChain->Present(1, 0); // Present with vsync
        //ctx.pSwapChain->Present(0, 0); // Present without vsync
    }
}


namespace dx11
{
    // https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples

    struct TextureId { int value = -1; };

    
    class Texture
    {
    public:
        ID3D11Texture2D* pTexture = 0;
        ID3D11ShaderResourceView* srv = 0;
        TextureId id;

        int image_width;
        int image_height;
        void* image_data;
    };

    template <size_t N>
    class TextureList
    {
    public:
        static constexpr size_t count = N;

        Texture data[count] = { 0 };

        Texture& get_dx_texture(TextureId id) { return data[id.value]; }

        ImTextureID get_imgui_texture(TextureId id) { return (ImTextureID)get_dx_texture(id).srv; }
    };


    template <size_t N>
    static inline TextureList<N> create_textures()
    {
        TextureList<N> textures{};

        for (int i = 0; i < textures.count; i++)
        {
            auto& texture = textures.data[i];
            texture.id.value = i;
        }

        return textures;
    }


    template <typename P>
    static inline void init_texture(P* data, int width, int height, Texture& texture, Context& ctx)
    {
        static_assert(sizeof(P) == 4);

        texture.image_data = (void*)data;
        texture.image_width = width;
        texture.image_height = height;
        
        auto& pTexture = texture.pTexture;
        auto& srv = texture.srv;

        D3D11_TEXTURE2D_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;        

        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = (void*)data;
        subResource.SysMemPitch = desc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        ctx.pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        ctx.pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &srv);

        ctx.pd3dDeviceContext->PSSetShaderResources(0, 1, &srv);
    }
    

    static inline void render_texture(Texture& texture, Context& ctx)
    {
        if (!texture.pTexture || !texture.srv)
        {
            return;
        }

        ctx.pd3dDeviceContext->UpdateSubresource(
            texture.pTexture, 
            0, 0, 
            texture.image_data,
            texture.image_width * 4, 
            0);

        ctx.pd3dDeviceContext->PSSetShaderResources(0, 1, &texture.srv);
    }
}