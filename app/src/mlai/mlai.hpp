#pragma once

#include "../../../libs/mnist/mnist.hpp"
#include "../../../libs/nn/nn_mlp.hpp"

#include <functional>


namespace mlai
{
    using bool_f = std::function<bool()>;


    class AI_State
    {
    public:

        mnist::ImageData train_data;
        mnist::LabelData train_labels;

        mnist::ImageData test_data;
        mnist::LabelData test_labels;

        nn::Net mlp;

        f32 train_error = 1.0f;
        f32 test_error = 1.0f;

        u32 data_id = 0;
        u32 epoch_id = 0;
        b8 prediction_ok = 0;
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

    void test(AI_State& state);


    void eval_at(AI_State& state, u32 data_id);
}