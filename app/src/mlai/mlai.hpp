#pragma once

#include "../../../libs/mnist/mnist.hpp"
#include "../../../libs/nn/nn_mlp.hpp"


namespace mlai
{
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
}