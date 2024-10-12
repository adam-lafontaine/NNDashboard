#pragma once

#include "../../../libs/mnist/mnist.hpp"
#include "../../../libs/nn/nn_mlp.hpp"

#include <functional>


namespace mlai
{
    using bool_f = std::function<bool()>;
    using Span32 = mlp::Span32;


    constexpr int TRAIN_ALL_LABELS = -1;


    class AI_State
    {
    public:

        int train_label = TRAIN_ALL_LABELS;

        mnist::ImageData train_image_data;
        mnist::LabelData train_label_data;

        mnist::ImageData test_image_data;
        mnist::LabelData test_label_data;

        img::GrayView cnn_gradient;
        img::GrayView cnn_pool;

        mlp::NetTopology topology{};
        mlp::Net mlp;

        f32 train_error = 1.0f;
        f32 test_error = 1.0f;

        u32 data_id = 0;
        u32 epoch_id = 0;
        b8 prediction_ok = 0;

        img::Buffer8 cnn_buffer;
    };


    class DataFiles
    {
    public:
        cstr train_data_path;
        cstr test_data_path;
        cstr train_labels_path;
        cstr test_labels_path;
    };


    bool load_data(AI_State& state, DataFiles files);

    void destroy(AI_State& state);

    void train(AI_State& state, bool_f const& train_condition);

    void test(AI_State& state, bool_f const& test_condition);
}