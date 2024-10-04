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
    enum class DataStatus : u8
    {
        NotLoaded = 0,
        InProgress,
        Loaded,
        Fail
    };


    enum class MLStatus : u8
    {
        None = 0,
        Training
    };


    class DisplayState
    {
    public:

        DataStatus ai_data_status = DataStatus::NotLoaded;
        MLStatus ai_status = MLStatus::None;

        mlai::AI_State ai_state{};

        img::Image input_image;
        img::SubView input_view;
        ImTextureID input_texture = 0;

        mlai::DataFiles ai_files;

        nn::NetTopology topology{};
    };


    inline void destroy(DisplayState& state)
    {
        mlai::destroy(state.ai_state);
        img::destroy_image(state.input_image);
    }


    inline bool init(DisplayState& state)
    {
        u32 display_width = 480;
        u32 display_height = 320;

        if (!img::create_image(state.input_image, display_width, display_height, "input_image"))
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


    template <u32 N>
    class ImGuiLabelArray
    {
    public:
        char labels[N][32] = { 0 };
    };


    template <u32 N>
    constexpr static ImGuiLabelArray<N> make_imgui_labels(cstr base)
    {
        ImGuiLabelArray<N> labels{};

        auto len = span::strlen(base);

        for (u32 i = 0; i < N; i++)
        {
            auto dst = labels.labels[i];
            for (u32 b = 0; b < len; b++)
            {
                dst[b] = base[b];
            }
            dst[len] = 'A' + i;
        }

        return labels;
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


    static bool load_data(DisplayState& state)
    {
        auto& ai = state.ai_state;
        if (!mlai::load_data(ai, state.ai_files))
        {
            return false;
        }

        state.topology.set_input_size(mnist::data_at(ai.test_data, 0).length);
        state.topology.set_output_size(mnist::data_at(ai.test_labels, 0).length);

        return true;
    }
    
    
    static void load_ai_data_async(DisplayState& state)
    {
        using DS = DataStatus;

        auto const load = [&]()
        {
            state.ai_data_status = DS::InProgress;
            auto ok = load_data(state);
            ok &= create_input_display(state);

            state.ai_data_status = ok ? DS::Loaded : DS::Fail;
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


    static void start_ai_training(DisplayState& state)
    {
        state.ai_status = MLStatus::Training;

        auto const condition = [&](){ return state.ai_status == MLStatus::Training; };

        mlai::train(state.ai_state, condition);
    }


    static void start_ai_training_async(DisplayState& state)
    {
        std::thread th([&](){ start_ai_training(state); });
        th.detach();
    }


    static void stop_ai_training(DisplayState& state)
    {
        state.ai_status = MLStatus::None;
    }

} // internal

} // display


namespace display
{
    static void status_window(DisplayState& state)
    {
        using DS = DataStatus;

        auto is_disabled = state.ai_data_status == DS::Loaded;

        ImGui::Begin("Status");

        if (is_disabled)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Load Data") && state.ai_data_status == DS::NotLoaded)
        {
            internal::load_ai_data_async(state);            
        }

        if (is_disabled)
        {
            ImGui::EndDisabled();
        }

        cstr msg = "";
        switch (state.ai_data_status)
        {
        case DS::Fail:
            msg = "ERROR";
            break;

        case DS::InProgress:
            msg = "Loading...";
            break;

        case DS::Loaded:
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
    
    
    static void inspect_data_window(DisplayState& state)
    {
        ImGui::Begin("Inspect");

        auto w = state.input_image.width;
        auto h = state.input_image.height;

        if (state.ai_data_status != DataStatus::Loaded)
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


    static void topology_window(DisplayState& state)
    {
        constexpr auto N = nn::NetTopology::MAX_INNER_LAYERS;

        constexpr auto layer_labels_array = internal::make_imgui_labels<N>("##LayerLabels");

        auto layer_labels = layer_labels_array.labels;

        auto& topology = state.topology;
        auto& mlp = state.ai_state.mlp;

        auto is_disabled = mlp.memory.ok;


        ImGui::Begin("Topology");

        constexpr int layer_size_min = 1;
        constexpr int layer_size_max = 32;
        constexpr int layer_size_default = 16;

        
        static int n_inner_layers = layer_size_min;
        static int inner_layers[N] = { 0 };

        ImGui::SliderInt("Inner layers", &n_inner_layers, 1, (int)N);

        ImGui::Text("%u", topology.get_input_size());
        ImGui::SameLine();

        if (is_disabled)
        {
            ImGui::BeginDisabled();
        }

        for (int i = 0; i < n_inner_layers; i++)
        {
            inner_layers[i] = inner_layers[i] ? num::max(layer_size_min, inner_layers[i]) : layer_size_default;
            ImGui::VSliderInt(layer_labels[i], ImVec2(18, 160), inner_layers + i, layer_size_min, layer_size_max);
            ImGui::SameLine();
        }

        if (is_disabled)
        {
            ImGui::EndDisabled();
        }

        ImGui::Text("%u", topology.get_output_size());

        topology.set_inner_layers((u32)n_inner_layers);
        
        for (int i = 0; i < n_inner_layers; i++)
        {
            topology.set_inner_size_at((u32)inner_layers[i], { u8(i) });
        }

        ImGui::Text("Bytes: %u", nn::mlp_bytes(topology));

        if (state.ai_data_status == DataStatus::Loaded)
        {
            if (is_disabled)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Create"))
            {
                nn::create(mlp, topology);
            }

            if (is_disabled)
            {
                ImGui::EndDisabled();
                ImGui::SameLine();
                ImGui::Text("OK");
            }
        }

        ImGui::End();
    }


    static void train_window(DisplayState& state)
    {
        auto& mlp = state.ai_state.mlp;

        auto start_disabled = !mlp.memory.ok || state.ai_status == MLStatus::Training;
        auto stop_disabled = state.ai_status != MLStatus::Training;

        ImGui::Begin("Train");

        if (start_disabled) { ImGui::BeginDisabled(); }

        if (ImGui::Button("Start"))
        {
            internal::start_ai_training_async(state);
        }

        if (start_disabled) { ImGui::EndDisabled(); }

        ImGui::SameLine();

        if (stop_disabled) { ImGui::BeginDisabled(); }

        if (ImGui::Button("Stop"))
        {
            internal::stop_ai_training(state);
        }

        if (stop_disabled) { ImGui::EndDisabled(); }

        constexpr int data_count = 256;
        constexpr f32 plot_min = 0.0f;
        constexpr f32 plot_max = 1.0f;
        constexpr auto plot_size = ImVec2(0, 80.0f);
        constexpr auto data_stride = sizeof(f32);

        static f32 plot_data[data_count] = { 0 };
        static u8 data_offset = 0;

        cstr overlay = "";

        plot_data[data_offset++] = state.ai_state.train_error;

        ImGui::PlotLines("##ErrorPlot", 
            plot_data, 
            data_count, 
            (int)data_offset,
            overlay,
            plot_min, plot_max,
            plot_size,
            data_stride);

        ImGui::End();
    }


    static void activation_window(DisplayState& state)
    {
        auto& ai = state.ai_state;
        auto& net = ai.mlp;
        auto& layers = net.layers.data;        

        int table_flags = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV;
        auto table_dims = ImVec2(0.0f, 0.0f);

        ImGui::Begin("Activations");

        if (!net.memory.ok)
        {
            ImGui::End();
            return;
        }

        int n_columns = net.layers.length;
        int n_rows = 1;

        if (ImGui::BeginTable("ActivationTable", n_columns, table_flags, table_dims))
        {
            for (int c = 0; c < n_columns; c++)
            {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 50.0f);

                auto len = (int)layers[c].io_back.length;
                if (len > n_rows)
                {
                    n_rows = len;
                }
            }

            for (int i = 0; i < n_rows; i++)
            {
                ImGui::TableNextRow();
                for (int c = 0; c < n_columns; c++)
                {
                    ImGui::TableSetColumnIndex(c);

                    auto& io = layers[c].io_back;

                    if (i < io.length)
                    {
                        ImGui::Text("%6.4f", io.activation[i]);
                    }
                    else
                    {
                        ImGui::Text("");
                    }
                }
            }

            ImGui::EndTable();
        }

        static u32 data_id = 0;

        if (ImGui::Button("Next"))
        {
            data_id++;
            if (data_id > ai.train_data.image_count)
            {
                data_id = 0;
            }

            mlai::eval_at(ai, data_id);
        }

        ImGui::End();
    }
}


namespace display
{
    inline void show_display(DisplayState& state)
    {
        status_window(state);
        inspect_data_window(state);
        topology_window(state);
        train_window(state);

        activation_window(state);
    }
}
