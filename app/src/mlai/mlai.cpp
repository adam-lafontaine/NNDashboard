#pragma once

#include "mlai.hpp"


namespace mlai
{
    static u32 increment_wrap(u32 value, u32 max_value)
    {
        value += 1;
        return value > max_value ? 0 : value;
    }
}


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


    void destroy(AI_State& state)
    {
        mnist::destroy_data(state.train_data);
        mnist::destroy_data(state.test_data);
        mnist::destroy_data(state.train_labels);
        mnist::destroy_data(state.test_labels);
        nn::destroy(state.mlp);
    }


    void train(AI_State& state, bool_f const& train_condition)
    {
        auto& data = state.train_data;
        auto& labels = state.train_labels;
        u32 data_count = data.image_count;
        u32 data_id = 0;

        auto& mlp = state.mlp;

        while (train_condition())
        {
            auto input = mnist::data_at(data, data_id);
            auto expected = mnist::data_at(labels, data_id);
            
            nn::update(mlp, input, expected);

            data_id = increment_wrap(data_id, data_count - 1);
        }
    }
}