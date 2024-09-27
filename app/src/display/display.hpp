#pragma once

//#include "../../../libs/imgui/imgui.h"
#include "../../../libs/image/image.hpp"
#include "../mlai/mlai.hpp"

#include <cassert>
#include <thread>
//#include <functional>

namespace img = image;


namespace display
{
    enum class LoadStatus : u8
    {
        NotLoaded = 0,
        InProgress,
        Loaded,
        Fail
    };


    class DisplayState
    {
    public:

        LoadStatus ai_load_status = LoadStatus::NotLoaded;

        mlai::DataFiles ai_files;

        mlai::AI_State ai_state{};

        img::ImageView input_view;
        void* input_texture = 0;

        img::Buffer32 pixel_data;
    };


    inline void destroy(DisplayState& state)
    {
        mlai::destroy_data(state.ai_state);

        mb::destroy_buffer(state.pixel_data);
    }
}

/* internal */

namespace display
{
namespace internal
{
    static void load_ai_data_async(DisplayState& state)
    {
        using LS = LoadStatus;

        auto const load = [&]()
        {
            state.ai_load_status = LS::InProgress;
            auto ok = mlai::load_data(state.ai_state, state.ai_files);



            state.ai_load_status = ok ? LS::Loaded : LS::Fail;
        };

        std::thread th(load);
        th.detach();
    }


    static void scale_view(img::GrayView const& src, img::ImageView const& dst)
    {
        assert(dst.width % src.width == 0);
        assert(dst.height % src.height == 0);

        auto ws = dst.width / src.width;
        auto hs = dst.height / src.height;

        auto rect = img::make_rect(ws, hs);
        auto sub = img::sub_view(dst, rect);

        for (u32 y = 0; y < src.height; y++)
        {
            auto row = img::row_begin(src, y);
            for (u32 x = 0; x < src.width; y++)
            {
                auto gray = row[x];
                sub = img::sub_view(dst, rect);

                img::fill(sub, img::to_pixel(gray));

                rect.x_begin += ws;
                rect.x_end += ws;
            }

            rect.x_begin = 0;
            rect.x_end = ws;
            rect.y_begin += hs;
            rect.y_end += hs;
        }
    }


    static void image_data_properties(mnist::ImageData const& data, cstr title)
    {
        ImGui::Text(title);
        ImGui::Text(" count: %u", data.image_count);
        ImGui::Text(" width: %u", data.image_width);
        ImGui::Text("height: %u", data.image_height);

    }


    static void label_data_properties(mnist::LabelData const& data, cstr title)
    {
        ImGui::Text(title);
        ImGui::Text(" count: %u", data.label_count);
    }

} // internal

} // display


namespace display
{
    inline void status_window(DisplayState& state)
    {
        using LS = LoadStatus;

        ImGui::Begin("Status");

        if (state.ai_load_status == LS::NotLoaded)
        {
            if (ImGui::Button("Load Data"))
            {
                internal::load_ai_data_async(state);
            }
            else
            {
                cstr msg = "NA";

                switch (state.ai_load_status)
                {
                case LS::NotLoaded:
                    msg = "NA";
                    break;
                
                case LS::InProgress:
                    msg = "Loading...";
                    break;
                
                default:
                    msg = "Error";
                    break;
                }

                ImGui::Text(msg);
            }
        }

        ImGui::BeginGroup();
        internal::image_data_properties(state.ai_state.train_data, "Training data");
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        internal::image_data_properties(state.ai_state.test_data, "Testing data");
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        internal::label_data_properties(state.ai_state.train_labels, "Training labels");
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        internal::label_data_properties(state.ai_state.test_labels, "Testing labels");
        ImGui::EndGroup();

        ImGui::End();
    }
    
    
    inline void input_image_window(DisplayState& state)
    {
        if (state.ai_load_status != LoadStatus::Loaded)
        {
            return;
        }

        u32 image_id = 4;
        f32 scale = 1.0f;

        auto& view = state.input_view;

        auto src_data = state.ai_state.train_data;
        auto src_gray = mnist::image_at(src_data, image_id);

        internal::scale_view(src_gray, state.input_view);

        auto w = view.width * scale;
        auto h = view.height * scale;

        ImGui::Begin("Input Image");

        ImGui::Image(state.input_texture, ImVec2(w, h));

        ImGui::End();
    }
}