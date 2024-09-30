#pragma once

#include "../../../libs/mnist/mnist.hpp"


namespace mlai
{
    class AI_State
    {
    public:

        mnist::ImageData train_data;
        mnist::ImageData test_data;

        mnist::LabelData train_labels;
        mnist::LabelData test_labels;

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

    void destroy_data(AI_State& state);



}