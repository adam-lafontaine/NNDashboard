#pragma once

#include "../span/span.hpp"


namespace nn
{
    using Span32 = SpanView<f32>;
    using Matrix32 = MatrixView2D<f32>;


    class Layer
    {
    public:

        Span32 act_in;
        Span32 act_out;

        Matrix32 weights;
        Span32 bias;

        Span32 error_in;
        Span32 error_out;
    };


    class NetTopology
    {
    public:
        constexpr static u32 MAX_LAYERS = 16;

        u32 n_layers = 0;

        u32 input_size = 0;

        u32 layer_sizes[MAX_LAYERS] = { 0 };

        u32 output_size = 0;
    };


    class MultiLayerPerceptron
    {
    public:

        Span32 input;
        Span32 output;
        Span32 error;

        SpanView<Layer> layers; // topology

        MemoryBuffer<f32> memory;
    };

    using Net = MultiLayerPerceptron;


    inline void destroy(Net& net)
    {
        mb::destroy_buffer(net.memory);
    }


    void create(Net& net, NetTopology const& layer_sizes);

    void eval(Net const& net, Span32 const& input);

    void update(Net const& net, Span32 const& input, Span32 const& expected);

    u32 mlp_bytes(NetTopology const& topology);
}