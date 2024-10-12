#pragma once

#include "mlai.hpp"


namespace mlai
{
    static u32 increment_wrap(u32 value, u32 max_value)
    {
        value += 1;
        return value > max_value ? 0 : value;
    }


    static void cnn_convert(img::GrayView const& src, img::GrayView grad, img::GrayView pool, Span32 const& dst)
    {
        /*img::GrayView grad{};        
        grad.width = src.width - 2;
        grad.height = src.height - 2;

        img::GrayView pool{};        
        pool.width = grad.width / 2;
        pool.height = grad.height / 2;
        */
        
        assert("*** bad gradient image dimensions ***" && grad.width == src.width - 2 && grad.height == src.height - 2);
        assert("*** bad pooling image dimensions ***" && pool.width == grad.width / 2 && pool.height == grad.height / 2);
        assert("*** f32 buffer wrong size ***" && dst.length == 2 * pool.width * pool.height);
                
        auto const len = pool.width * pool.height;

        constexpr auto F = 1.0f / 255.0f;        

        img::gradient_x(src, grad);
        img::scale_down_max(grad, pool);

        auto d = dst.data;
        for (u32 i = 0; i < len; i++)
        {
            d[i] = F * pool.matrix_data_[i];
        }

        img::gradient_y(src, grad);
        img::scale_down_max(grad, pool);

        d += len;
        for (u32 i = 0; i < len; i++)
        {
            d[i] = F * pool.matrix_data_[i];
        }
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

        auto ok = state.train_image_data.ok &&
            state.test_image_data.ok &&
            state.train_label_data.ok &&
            state.test_label_data.ok;
        
        if (!ok)
        {
            return false;
        }

        auto w = state.train_image_data.image_width;
        auto h = state.train_image_data.image_height;

        auto w_gradient = w - 2;
        auto h_gradient = h - 2;

        auto w_pool = w_gradient / 2;
        auto h_pool = h_gradient / 2;

        auto cnn_pixels = w_gradient * h_gradient + w_pool * h_pool;
        state.cnn_buffer = img::create_buffer8(cnn_pixels, "cnn pixels");
        if (!state.cnn_buffer.ok)
        {
            return false;
        }

        state.cnn_gradient = img::make_view(w_gradient, h_gradient, state.cnn_buffer);
        state.cnn_pool = img::make_view(w_pool, h_pool, state.cnn_buffer);

        state.topology.set_input_size(2 * w_pool * h_pool);

        return true;
    }


    void destroy(AI_State& state)
    {
        mnist::destroy_data(state.train_image_data);
        mnist::destroy_data(state.test_image_data);
        mnist::destroy_data(state.train_label_data);
        mnist::destroy_data(state.test_label_data);
        mb::destroy_buffer(state.cnn_buffer);
        mlp::destroy(state.mlp);
    }


    void train(AI_State& state, bool_f const& train_condition)
    {
        auto& data = state.train_image_data;
        auto& labels = state.train_label_data;

        auto& grad = state.cnn_gradient;
        auto& pool = state.cnn_pool;
        auto& mlp = state.mlp;
        auto& mlp_input = mlp.input;

        u32 data_count = data.image_count;
        state.data_id = 0;
        state.epoch_id = 0;

        

        std::function<Span32()> get_expected;
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
            auto image = mnist::image_at(data, state.data_id);

            cnn_convert(image, grad, pool, mlp_input);
            auto expected = get_expected();
            
            mlp::update(mlp, expected);

            state.train_error = mlp::abs_error(mlp);

            auto p = mlp::prediction_label(mlp);

            state.prediction_ok = p >= 0 && expected.data[p] > 0.5f;

            state.data_id = increment_wrap(state.data_id, data_count - 1);
            state.epoch_id += state.data_id == 0;
        }
    }


    void test(AI_State& state, bool_f const& test_condition)
    {
        auto& data = state.test_image_data;
        auto& labels = state.test_label_data;

        auto& grad = state.cnn_gradient;
        auto& pool = state.cnn_pool;
        auto& mlp = state.mlp;
        auto& mlp_input = mlp.input;

        u32 data_count = data.image_count;
        state.data_id = 0;
        state.epoch_id = 0;

        std::function<Span32()> get_expected;
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
            auto image = mnist::image_at(data, state.data_id);

            cnn_convert(image, grad, pool, mlp_input);
            auto expected = get_expected();

            mlp::eval(mlp, expected);

            state.test_error = mlp::abs_error(mlp);

            auto p = mlp::prediction_label(mlp);

            state.prediction_ok = p >= 0 && expected.data[p] > 0.5f;

            state.data_id = increment_wrap(state.data_id, data_count - 1);
        }
    }
}