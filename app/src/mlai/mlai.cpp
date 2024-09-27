#pragma once

#include "mlai.hpp"


namespace mlai
{
    


    bool load_data(AI_State& state, DataFiles files)
    {
        state.train_data = mnist::load_image_data(files.train_data_path);
        state.test_data = mnist::load_image_data(files.test_data_path);
        state.train_labels = mnist::load_label_data(files.train_labels_path);
        state.test_labels = mnist::load_label_data(files.test_labels_path);

        return state.train_data.ok &&
            state.test_data.ok &&
            state.train_data.ok &&
            state.train_labels.ok;
    }


    void destroy_data(AI_State& state)
    {
        mnist::destroy_data(state.train_data);
        mnist::destroy_data(state.test_data);
        mnist::destroy_data(state.train_labels);
        mnist::destroy_data(state.test_labels);
    }
}