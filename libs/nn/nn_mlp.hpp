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


    void create(Net& net, SpanView<u32> const& layer_sizes);

    void eval(Net const& net, Span32 const& input);

    void update(Net const& net, Span32 const& input, Span32 const& expected);
}