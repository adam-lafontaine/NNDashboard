#pragma once

#include "nn_mlp.hpp"
#include "../util/numeric.hpp"

#include <cmath>
#include <cstdlib>


namespace mlp
{
    namespace num = numeric;


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


    static void softmax(Span32 const& span)
    {
        f32 max = 0.0f;
        for (u32 i = 0; i < span.length; i++)
        {
            if (span.data[i] > max)
            {
                max = span.data[i];
            }
        }

        f32 total = 0.0f;

        for (u32 i = 0; i < span.length; i++)
        {
            span.data[i] = std::exp(span.data[i] - max);
            total += span.data[i];
        }

        assert(total >= 0.0f);

        auto f = 1.0f / total;

        for (u32 i = 0; i < span.length; i++)
        {
            span.data[i] *= f;
        }
    }
    
    
    static void eval_forward(Layer const& layer)
    {
        auto input = layer.io_front;
        auto output = layer.io_back;

        auto a_in = activation_span(input);
        
        for (u32 o = 0; o < output.length; o++)
        {
            auto w = row_span(layer.weights, o);

            auto dot = span::dot(w, a_in);

            auto sum = dot + output.bias[o];

            // reLU
            output.activation[o] = sum < 0.0f ? 0.0f : sum;
        }
    }


    static void update_back(Layer const& layer)
    {
        auto front = layer.io_front;
        auto back = layer.io_back;

        f32 eta = 0.000001f;

        for (u32 b = 0; b < back.length; b++)
        {
            back.delta[b] = (back.activation[b] > 0.0f) ? back.error[b] : 0.0f;
            back.bias[b] += eta * back.delta[b];
        }

        for (u32 f = 0; f < front.length; f++)
        {
            auto& ef = front.error[f];
            auto af = front.activation[f];

            ef = 0.0f;
            for (u32 b = 0; b < back.length; b++)
            {
                auto& w = row_span(layer.weights, b).data[f];
                ef += back.delta[b] * w;
                w += eta * back.delta[b] * af;
            }
        }
    }


    static void update_input(Layer const& layer)
    {
        auto front = layer.io_front;
        auto back = layer.io_back;

        f32 eta = 0.000001f;

        for (u32 b = 0; b < back.length; b++)
        {
            back.delta[b] = (back.activation[b] > 0.0f) * back.error[b];
            back.bias[b] += eta * back.delta[b];
        }

        for (u32 f = 0; f < front.length; f++)
        {
            auto af = front.activation[f];

            for (u32 b = 0; b < back.length; b++)
            {
                auto& w = row_span(layer.weights, b).data[f];
                w += eta * back.delta[b] * af;
            }
        }
    }


    static u32 mlp_element_count(NetTopology topology)
    {
        u32 n_activation = 0;
        u32 n_bias = 0;
        u32 n_error = 0;
        u32 n_delta = 0;
        u32 n_weights = 0;
        
        // input layer
        TopologyIndex t_id = { (u8)0 };
        auto len_front = topology.get_input_size();
        auto len_back = topology.get_inner_size_at(t_id);

        n_activation += len_front;

        n_weights += len_front * len_back;
        n_bias += len_back;
        n_activation += len_back;
        n_error += len_back;
        n_delta += len_back;

        // inner layers
        auto N = topology.get_inner_layers();
        for (u32 i = 1; i < N; i++)
        {
            t_id.value++;

            len_front = len_back;
            len_back = topology.get_inner_size_at(t_id);

            n_weights += len_front * len_back;
            n_bias += len_back;
            n_activation += len_back;
            n_error += len_back;
            n_delta += len_back;
        }

        // output layer
        len_front = len_back;
        len_back = topology.get_output_size();

        n_weights += len_front * len_back;
        n_bias += len_back;
        n_activation += len_back;
        n_error += len_back;
        n_delta += len_back;

        return n_activation +
            n_weights + 
            n_bias + 
            n_error +
            n_delta;
    }


    
}


namespace mlp
{
    u32 mlp_bytes(NetTopology const& topology)
    {
        return mlp_element_count(topology) * sizeof(f32);
    }


    void create(Net& net, NetTopology topology)
    { 
        auto& buffer = net.memory;
        if (!mb::create_buffer(buffer, mlp_element_count(topology), "mlp"))
        {
            assert("*** mlp buffer failed ***" && false);
        }

        auto view = span::make_view(buffer);
        for (u32 i = 0; i < view.length; i++)
        {
            view.data[i] = (f32)rand() / RAND_MAX;
        }

        net.layers.data = net.layer_data;        
        auto& layers = net.layers.data;

        auto N = topology.get_inner_layers();
        net.layers.length = N + 1;

        // input layer
        TopologyIndex t_id = { (u8)0 };
        auto len_front = topology.get_input_size();
        auto len_back = topology.get_inner_size_at(t_id);
        {
            auto& layer = layers[0];

            auto& front = layer.io_front;
            auto& back = layer.io_back;

            front.length = len_front;
            front.activation = mb::push_elements(buffer, len_front);
            front.bias = 0;
            front.error = 0;
            front.delta = 0;

            back.length = len_back;
            back.activation = mb::push_elements(buffer, len_back);
            back.bias = mb::push_elements(buffer, len_back);
            back.error = mb::push_elements(buffer, len_back);
            back.delta = mb::push_elements(buffer, len_back);

            layer.weights = push_matrix(len_front, len_back, buffer);
            
            span::fill(span::to_span(back.error, len_back), 0.0f);

            net.input = span::to_span(front.activation, len_front);
        }

        // inner layers        
        u32 layer_id = 1;
        for (u32 i = 1; i < N; i++)
        { 
            t_id.value++;

            auto& layer = layers[layer_id];
            layer.io_front = layers[layer_id - 1].io_back;

            auto& front = layer.io_front;
            auto& back = layer.io_back;

            len_front = front.length;
            len_back = topology.get_inner_size_at(t_id);
            
            back.length = len_back;
            back.activation = mb::push_elements(buffer, len_back);
            back.bias = mb::push_elements(buffer, len_back);
            back.error = mb::push_elements(buffer, len_back);
            back.delta = mb::push_elements(buffer, len_back);

            layer.weights = push_matrix(len_front, len_back, buffer);
            
            span::fill(span::to_span(back.error, len_back), 0.0f);

            ++layer_id;
        }

        layer_id = net.layers.length - 1;

        // output layer
        {
            auto& layer = layers[layer_id];
            layer.io_front = layers[layer_id - 1].io_back;

            auto& front = layer.io_front;
            auto& back = layer.io_back;

            len_front = front.length;
            len_back = topology.get_output_size();
            
            back.length = len_back;
            back.activation = mb::push_elements(buffer, len_back);
            back.bias = mb::push_elements(buffer, len_back);
            back.error = mb::push_elements(buffer, len_back);
            back.delta = mb::push_elements(buffer, len_back);

            layer.weights = push_matrix(len_front, len_back, buffer);
            
            span::fill(span::to_span(back.error, len_back), 0.0f);

            net.output = span::to_span(back.activation, len_back);
            net.error = span::to_span(back.error, len_back);
        }

        // TODO?:
        // push all weights to the end of the buffer to enable saving a model

        assert(buffer.size_ == buffer.capacity_);
    }


    void eval(Net const& net)
    {
        for (u32 i = 0; i < net.layers.length; i++)
        {
            eval_forward(net.layers.data[i]);
        }

        softmax(net.output);
    }


    void eval(Net const& net, Span32 const& expected)
    {
        eval(net);

        span::sub(expected, net.output, net.error);
    }


    void update(Net const& net, Span32 const& expected)
    {
        eval(net, expected);

        auto N = net.layers.length;

        for (int i = N - 1; i > 0; i--)
        {
            auto& layer = net.layers.data[i];
            update_back(layer);
        }

        update_input(net.layers.data[0]);
    }


    int prediction_label(Net const& net)
    {
        for (u32 i = 0; i < net.output.length; i++)
        {
            if (net.output.data[i] > 0.8f)
            {
                return (int)i;
            }
        }

        return -1;
    }


    f32 abs_error(Net const& net)
    {
        f32 e = 0.0f;
        for (u32 i = 0; i < net.error.length; i++)
        {
            e += num::abs(net.error.data[i]);
        }

        return e / net.error.length;
    }
}
