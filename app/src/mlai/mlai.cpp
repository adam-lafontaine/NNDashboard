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
        state.train_image_data = mnist::load_image_data(files.train_data_path);
        state.test_image_data = mnist::load_image_data(files.test_data_path);
        state.train_label_data = mnist::load_label_data(files.train_labels_path);
        state.test_label_data = mnist::load_label_data(files.test_labels_path);

        return state.train_image_data.ok &&
            state.test_image_data.ok &&
            state.train_label_data.ok &&
            state.test_label_data.ok;
    }


    void destroy(AI_State& state)
    {
        mnist::destroy_data(state.train_image_data);
        mnist::destroy_data(state.test_image_data);
        mnist::destroy_data(state.train_label_data);
        mnist::destroy_data(state.test_label_data);
        nn::destroy(state.mlp);
    }


    void train(AI_State& state, bool_f const& train_condition)
    {
        auto& data = state.train_image_data;
        auto& labels = state.train_label_data;

        u32 data_count = data.image_count;
        state.data_id = 0;
        state.epoch_id = 0;

        auto& mlp = state.mlp;

        std::function<nn::Span32()> get_expected;
        if (state.train_label == TRAIN_ALL_LABELS)
        {
            get_expected = [&](){ return mnist::label_data_at(labels, state.data_id); };
        }
        else
        {
            get_expected = [&](){ return mnist::label_equals_at(labels, (u8)state.train_label, state.data_id); };
        }

        while (train_condition())
        {
            auto input = mnist::input_data_at(data, state.data_id);
            auto expected = get_expected();
            
            nn::update(mlp, input, expected);

            state.train_error = nn::abs_error(mlp);

            auto p = nn::prediction(mlp);

            state.prediction_ok = p >= 0 && expected.data[p] > 0.5f;

            state.data_id = increment_wrap(state.data_id, data_count - 1);
            state.epoch_id += state.data_id == 0;
        }
    }


    void test(AI_State& state, bool_f const& test_condition)
    {
        auto& data = state.test_image_data;
        auto& labels = state.test_label_data;

        u32 data_count = data.image_count;
        state.data_id = 0;
        state.epoch_id = 0;

        auto& mlp = state.mlp;

        std::function<nn::Span32()> get_expected;
        if (state.train_label == TRAIN_ALL_LABELS)
        {
            get_expected = [&](){ return mnist::label_data_at(labels, state.data_id); };
        }
        else
        {
            get_expected = [&](){ return mnist::label_equals_at(labels, (u8)state.train_label, state.data_id); };
        }

        while (test_condition())
        {
            auto input = mnist::input_data_at(data, state.data_id);
            auto expected = get_expected();

            nn::eval(mlp, input, expected);

            state.test_error = nn::abs_error(mlp);

            auto p = nn::prediction(mlp);

            state.prediction_ok = p >= 0 && expected.data[p] > 0.5f;

            state.data_id = increment_wrap(state.data_id, data_count - 1);
        }
    }


    void eval_at(AI_State& state, u32 data_id)
    {
        auto& data = state.train_image_data;
        auto& labels = state.train_label_data;
        u32 data_count = data.image_count;

        auto& mlp = state.mlp;

        auto input = mnist::input_data_at(data, data_id);

        nn::eval(mlp, input);
    }
}