#pragma once

#include "nn_mlp.hpp"

#include <cmath>


namespace nn
{
    static Matrix32 push_matrix(u32 width, u32 height, MemoryBuffer<f32>& buffer)
    {
        Matrix32 mat{};
        mat.width = width;
        mat.height = height;
        mat.matrix_data_ = mb::push_elements(buffer, width * height);

        return mat;        
    }


    template <typename T>
    static SpanView<T> row_span(MatrixView2D<T> const& mat, u16 h)
    {
        SpanView<T> span{};

        span.length = mat.width;
        span.data = mat.matrix_data_ + h * mat.width;

        return span;
    }


    static inline Span32 activation_span(IO const& io)
    {
        Span32 span{};
        span.data = io.activation;
        span.length = io.length;

        return span;
    }
    
    
    static void eval_forward(Layer const& layer)
    {
        auto input = layer.io_front;
        auto output = layer.io_back;

        auto a_in = activation_span(input);
        
        for (u32 o = 0; o < output.length; o++)
        {
            auto w = row_span(layer.weights, o);            

            auto sum = span::dot(w, a_in) + input.bias[o];

            output.activation[o] = sum > 0.0f ? sum : 0.0f;
        }
    }


    static void update_back(Layer const& layer)
    {
        auto input = layer.io_back;
        auto output = layer.io_front;

        f32 eta = 0.15f;

        for (u32 o = 0; o < output.length; o++)
        {
            auto a_out = output.activation[o];
            if (a_out <= 0.0f)
            {
                continue;
            }

            auto& e_out = output.error[o];
            auto& b_out = output.bias[o];

            e_out = 0.0f;
            for (u32 i = 0; i < input.length; i++)
            {
                auto& w = row_span(layer.weights, i).data[o];
                auto e_in = input.error[i];

                e_out += e_in * w;
                w -= eta * e_in * a_out;
            }

            b_out -= eta * e_out;
        }
    }


    static u32 net_element_count(NetTopology topology)
    {
        auto span = topology.to_span();

        auto N = span.length;
        auto sizes = span.data;

        u32 n_activation = 0;
        u32 n_bias = 0;
        u32 n_error = 0;
        u32 n_weights = 0;

        // input layer
        auto n0 = sizes[0];
        auto n1 = sizes[1];

        n_activation += n0;
        n_weights += n0 * n1;
        n_bias += n1;
        n_activation += n1;
        n_error += n1;

        for (u32 i = 1; i < N - 1; i++)
        {
            n0 = sizes[i];
            n1 = sizes[i + 1];

            n_weights += n0 * n1;
            n_bias += n1;
            n_activation += n1;
            n_error += n1;
        }

        return n_activation +
            n_weights + 
            n_bias + 
            n_error;
    }


    static void softmax(Span32 const& span)
    {
        f32 total = 0.0f;

        for (u32 i = 0; i < span.length; i++)
        {
            span.data[i] = std::exp(span.data[i]);
            total += span.data[i];
        }

        auto f = total <= 0.0f ? 0.0f : 1.0f / total;

        for (u32 i = 0; i < span.length; i++)
        {
            span.data[i] *= f;
        }
    }
}


namespace nn
{
    u32 mlp_bytes(NetTopology const& topology)
    {
        return net_element_count(topology) * sizeof(f32);
    }


    void create(Net& net, NetTopology topology)
    { 
        auto& buffer = net.memory;
        if (!mb::create_buffer(buffer, net_element_count(topology), "mlp"))
        {
            assert("*** mlp buffer failed ***" && false);
        }

        span::fill_32(span::make_view(buffer), 0.5f);

        auto const span = topology.to_span();

        auto N = span.length;
        auto sizes = span.data;

        net.layers.length = N - 1;
        net.layers.data = net.layer_data;

        auto& layers = net.layers.data;        

        // input layer
        assert(sizes[0] > 0);
        auto n0 = sizes[0];
        auto n1 = sizes[1];

        {
            auto& layer = layers[0];

            layer.io_front.length = n0;
            layer.io_front.activation = mb::push_elements(buffer, n0);
            layer.io_front.bias = 0;
            layer.io_front.error = 0;

            layer.io_back.length = n1;
            layer.io_back.activation = mb::push_elements(buffer, n1);
            layer.io_back.bias = mb::push_elements(buffer, n1);
            layer.io_back.error = mb::push_elements(buffer, n1);

            layer.weights = push_matrix(n0, n1, buffer);
        }

        for (u32 i = 1; i < N - 1; i++)
        {
            assert(sizes[i] > 0);

            n0 = sizes[i];
            n1 = sizes[i + 1];

            auto& layer = layers[i];

            layer.io_front = layers[i - 1].io_back;

            layer.io_back.length = n1;
            layer.io_back.activation = mb::push_elements(buffer, n1);
            layer.io_back.bias = mb::push_elements(buffer, n1);
            layer.io_back.error = mb::push_elements(buffer, n1);

            layer.weights = push_matrix(n0, n1, buffer);
        }

        assert(buffer.capacity_ - buffer.size_ == 0);
        
        net.input = span::to_span(layers[0].io_front.activation, layers[0].io_front.length);
        net.output = span::to_span(layers[N - 1].io_back.activation, layers[N - 1].io_back.length);
        net.error = span::to_span(layers[N - 1].io_back.error, layers[N - 1].io_back.length);
    }


    void eval(Net const& net, Span32 const& input)
    {
        span::copy(input, net.input);

        for (u32 i = 0; i < net.layers.length; i++)
        {
            eval_forward(net.layers.data[i]);
        }

        softmax(net.output);
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
