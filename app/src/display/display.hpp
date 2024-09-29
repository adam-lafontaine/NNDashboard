#pragma once

#include "../../../libs/imgui/imgui.h"
#include "../../../libs/image/image.hpp"
#include "../../../libs/util/numeric.hpp"
#include "../mlai/mlai.hpp"

#include <cassert>
#include <thread>

namespace img = image;
namespace num = numeric;


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

        mlai::AI_State ai_state{};

        img::Image input_image;
        img::SubView input_view;
        ImTextureID input_texture = 0;

        mlai::DataFiles ai_files;
    };


    inline void destroy(DisplayState& state)
    {
        mlai::destroy_data(state.ai_state);
        img::destroy_image(state.input_image);
    }


    inline bool init(DisplayState& state)
    {
        u32 display_width = 480;
        u32 display_height = 320;

        if (!img::create_image(state.input_image, display_width, display_height))
        {
            return false;
        }

        return true;
    }
}

/* internal */

namespace display
{
namespace internal
{
    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
        {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }


    static bool create_input_display(DisplayState& state)
    {
        auto wd = state.ai_state.test_data.image_width; // 17
        auto hd = state.ai_state.test_data.image_height; // 11

        auto wi = state.input_image.width;
        auto hi = state.input_image.height;

        if (!wd || !hd || wi < wd || hi < hd)
        {
            return false;
        }

        auto scale = num::min(wi / wd, hi / hd);

        auto w = wd * scale;
        auto h = hd * scale;

        Rect2Du32 r{};
        r.x_begin = (wi - w) / 2;
        r.x_end = r.x_begin + w;
        r.y_begin = (hi - h) / 2;
        r.y_end = r.y_begin + h;

        auto view = img::make_view(state.input_image);
        img::fill(view, img::to_pixel(0));

        state.input_view = img::sub_view(view, r);

        return true;
    }


    static void load_ai_data_async(DisplayState& state)
    {
        using LS = LoadStatus;

        auto const load = [&]()
        {
            state.ai_load_status = LS::InProgress;
            auto ok = mlai::load_data(state.ai_state, state.ai_files);
            ok &= create_input_display(state);

            state.ai_load_status = ok ? LS::Loaded : LS::Fail;
        };

        std::thread th(load);
        th.detach();
    }


    static void scale_view(img::GrayView const& src, img::SubView const& dst)
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
            for (u32 x = 0; x < src.width; x++)
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
        ImGui::Text("%s", title);
        ImGui::Text(" count: %u", data.image_count);
        ImGui::Text(" width: %u", data.image_width);
        ImGui::Text("height: %u", data.image_height);

    }


    static void label_data_properties(mnist::LabelData const& data, cstr title)
    {
        ImGui::Text("%s", title);
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

        if (ImGui::Button("Load Data") && state.ai_load_status == LS::NotLoaded)
        {
            internal::load_ai_data_async(state);            
        }

        cstr msg = "";
        switch (state.ai_load_status)
        {
        case LS::Fail:
            msg = "ERROR";
            break;

        case LS::InProgress:
            msg = "Loading...";
            break;

        case LS::Loaded:
            msg = "OK";
            break;

        default:
            msg = "";
            break;
        }

        ImGui::SameLine();
        ImGui::Text("%s", msg);

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
    
    
    inline void inspect_data_window(DisplayState& state)
    {
        ImGui::Begin("Inspect");

        auto w = state.input_image.width;
        auto h = state.input_image.height;

        if (state.ai_load_status != LoadStatus::Loaded)
        {
            // empty image
            ImGui::Image(state.input_texture, ImVec2(w, h));

            ImGui::End();
            return;
        }

        auto& view = state.input_view;
        auto& ai = state.ai_state;

        auto src_data = ai.train_data;
        auto label_data = ai.train_labels;

        static int data_option = 0;
        ImGui::RadioButton("Training data", &data_option, 0); ImGui::SameLine();
        ImGui::RadioButton("Testing data", &data_option, 1);

        static int data_id = 0;

        if (data_option)
        {
            src_data = ai.test_data;
            label_data = ai.test_labels;
        }
        
        int data_id_min = 0;
        int data_id_max = src_data.image_count - 1;

        if (data_id > data_id_max)
        {
            data_id = data_id_max;
        }

        auto src_gray = mnist::image_at(src_data, (u32)data_id);

        internal::scale_view(src_gray, view);
        ImGui::Image(state.input_texture, ImVec2(w, h));

        internal::HelpMarker("CTRL+click to input value.");
        ImGui::SetNextItemWidth(-100);
        ImGui::SliderInt("Data index", &data_id, data_id_min, data_id_max);
        ImGui::Text("Label: %u", mnist::label_at(label_data, (u32)data_id));

        ImGui::End();
    }
}