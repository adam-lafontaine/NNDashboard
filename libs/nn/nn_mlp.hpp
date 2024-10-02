#pragma once

#include "../span/span.hpp"


namespace nn
{
    using Span32 = SpanView<f32>;
    using Matrix32 = MatrixView2D<f32>;


    class IO
    {
    public:
        f32* activation = 0;
        f32* bias = 0;
        f32* error = 0;

        u32 length;
    };


    class Layer
    {
    public:

        IO io_front;

        Matrix32 weights;

        IO io_back;
    };


    class NetTopology
    {
    public:
        constexpr static u32 MAX_INNER_LAYERS = 16;
        constexpr static u32 MAX_LAYERS = 1 + MAX_INNER_LAYERS + 1;

        u32 n_layers = 2;

        u32 layer_sizes[MAX_LAYERS] = { 0 };

        u32 get_input_size() { return layer_sizes[0]; }

        u32 get_output_size() { return layer_sizes[n_layers - 1]; }

        void set_input_size(u32 size) { layer_sizes[0] = size; }

        void set_output_size(u32 size) { layer_sizes[n_layers - 1] = size; }
    };


    class MultiLayerPerceptron
    {
    public:
        constexpr static u32 MAX_LAYERS = NetTopology::MAX_LAYERS;

        SpanView<Layer> layers;

        Span32 input;
        Span32 output;
        Span32 error;

        Layer layer_data[MAX_LAYERS];
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