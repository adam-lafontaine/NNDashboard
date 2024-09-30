#pragma once

#include "nn_mlp.hpp"


namespace nn
{
    static u32 net_element_count(SpanView<u32> const& layer_sizes)
    {
        auto N = layer_sizes.length - 1;
        auto sizes = layer_sizes.data;

        u32 act = 0;
        u32 error = 0;
        u32 weights = 0;
        u32 bias = 0;

        act = error = sizes[0];
        
        u32 count = act + error;

        for (u32 i = 1; i < N; i++)
        {
            act = error = sizes[i + 1];
            weights = sizes[i] * sizes[i + 1];
            bias = sizes[i];

            count += act + error + weights + bias;
        }

        return count;
    }


    Matrix32 push_matrix(u32 width, u32 height, MemoryBuffer<f32>& buffer)
    {
        Matrix32 mat{};
        mat.width = width;
        mat.height = height;
        mat.matrix_data_ = mb::push_elements(buffer, width * height);

        return mat;        
    }


    template <typename T>
    SpanView<T> row_span(MatrixView2D<T> const& mat, u16 h)
    {
        Span<T> span{};

        span.length = mat.width;
        span.data = mat.data + h * mat.width;

        return span;
    }


    void eval_forward(Layer const& layer)
    {
        auto& in = layer.act_in;
        auto& weights = layer.weights;
        auto& bias = layer.bias;
        auto& out = layer.act_out;

        auto in_len = in.length;
        auto out_len = out.length;

        for (u32 o = 0; o < out_len; o++)
        {
            auto w = row_span(weights, o);

            auto sum = span::dot(in, w) + bias.data[o];

            out.data[o] = sum > 0.0f ? sum : 0.0f;
        }
    }


    void update_back(Layer const& layer)
    {
        auto& in = layer.act_in;
        auto& weights = layer.weights;
        auto& bias = layer.bias;
        auto& error_in = layer.error_in;
        auto& error_out = layer.error_out;

        auto in_len = in.length;
        auto out_len = error_out.length;

        f32 eta = 0.15f;

        for (u32 i = 0; i < in_len; i++)
        {
            error_in.data[i] = 0.0f;
            if (in.data[i] <= 0.0f)
            {
                continue;
            }
            
            for (u32 o = 0; o < out_len; o++)
            {
                auto w = row_span(weights, o);
                error_in.data[i] += error_out.data[o] * w.data[i];
                w.data[i] -= eta * error_out.data[o] * in.data[i];
            }

            bias.data[i] -= eta * error_in.data[i];
        }
    }
}


namespace nn
{
    void create(Net& net, SpanView<u32> const& layer_sizes)
    {
        auto& buffer = net.memory;
        if (!mb::create_buffer(buffer, net_element_count(layer_sizes), "mlp"))
        {

        }

        auto N = layer_sizes.length - 1;
        auto sizes = layer_sizes.data;

        net.layers.length = N;

        auto layers = net.layers.data;

        u32 size32 = 0;

        for (u32 i = 0; i < N; i++)
        {        
            auto& layer = layers[i];

            if (i == 0)
            {
                layer.act_in = span::push_span(buffer, sizes[0]);
                layer.error_in = span::push_span(buffer, sizes[0]);
            }
            else
            {
                auto& prev = layers[i - 1];

                layer.act_in = prev.act_out;
                layer.error_in = prev.error_in;
            }

            layer.act_out = span::push_span(buffer, sizes[i + 1]);
            layer.error_out = span::push_span(buffer, sizes[i + 1]);

            layer.weights = push_matrix(layer.act_in.length, layer.act_out.length, buffer);
            layer.bias = span::push_span(buffer, layer.act_in.length);
        }
        
        net.input = layers[0].act_in;
        net.output = layers[N - 1].act_out;
        net.error = layers[N - 1].error_out;
    }


    void eval(Net const& net, Span32 const& input)
    {
        span::copy(input, net.input);

        for (u32 i = 0; i < net.layers.length; i++)
        {
            eval_forward(net.layers.data[i]);
        }
    }


    void update(Net const& net, Span32 const& input, Span32 const& expected)
    {
        eval(net, input);

        auto len = net.output.length;
        auto out = net.output.data;
        auto error = net.error.data;

        span::sub(expected, net.output, net.error);

        auto N = net.layers.length;

        for (u32 i = 0; i < N; i++)
        {
            auto& layer = net.layers.data[N - 1 - i];
            update_back(layer);
        }
    }
}