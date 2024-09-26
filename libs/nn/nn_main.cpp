using f32 = float;
using u32 = unsigned;
using u16 = unsigned short;
using u64 = unsigned long long;


template <class T>
class Span
{
public:
    u32 length = 0;

    T* data;
};


template <class T>
Span<T> make_span(u32 len)
{
    Span<T> span{};
    span.length = len;

    return span;
}


template <class T>
u64 size(Span<T> const& span)
{
    return sizeof(T) * span.length;
}


using Span32 = Span<f32>;


void copy(Span32 const& src, Span32 const& dst)
{
    auto len = src.length; // == dst.length

    for (u32 i = 0; i < len; i++)
    {
        dst.data[i] = src.data[i];
    }
}


void sub(Span32 const& a, Span32 const& b, Span32 const& dst)
{
    auto len = a.length; // == b.length == dst.length

    for (u32 i = 0; i < len; i++)
    {
        dst.data[i] = a.data[i] - b.data[i];
    }
}


f32 dot(Span32 const& a, Span32 const& b)
{
    auto len = a.length; // == b.length

    f32 res = 0.0f;
    for (u32 i = 0; i < len; i++)
    {
        res += a.data[i] * b.data[i];
    }

    return res;
}


template <class T>
class Matrix
{
public:
    u32 width = 0;
    u32 height = 0;
    T* data = 0;
};


template <class T>
Matrix<T> make_matrix(u32 width, u32 height)
{
    Matrix<T> mat{};
    mat.width = width;
    mat.height = height;

    return mat;
}


template <class T>
u32 size(Matrix<T> const& mat)
{
    return sizeof(T) * mat.width * mat.height;
}


template <typename T>
Span<T> row_span(Matrix<T> const& mat, u16 h)
{
    Span<T> span{};

    span.length = mat.width;
    span.data = mat.data + h * mat.width;

    return span;
}


using Matrix32 = Matrix<f32>;


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

        auto sum = dot(in, w) + bias.data[o];

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


class Net
{
public:

    Span32 input;
    Span32 output;
    Span32 error;

    Span<Layer> layers; // topology
};


u32 net_element_count(Span<u32> const& layer_sizes)
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


void create(Net& net, Span<u32> const& layer_sizes)
{
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
            layer.act_in = make_span<f32>(sizes[0]);
            layer.error_in = make_span<f32>(sizes[0]);            
        }
        else
        {
            auto& prev = layers[i - 1];

            layer.act_in = prev.act_out;
            layer.error_in = prev.error_in;
        }

        layer.act_out = make_span<f32>(sizes[i + 1]);
        layer.error_out = make_span<f32>(sizes[i + 1]);

        layer.weights = make_matrix<f32>(layer.act_in.length, layer.act_out.length);
        layer.bias = make_span<f32>(layer.act_in.length);
    }
    
    net.input = layers[0].act_in;
    net.output = layers[N - 1].act_out;
    net.error = layers[N - 1].error_out;
}


void eval(Net const& net, Span32 const& input)
{
    copy(input, net.input);

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

    sub(expected, net.output, net.error);

    auto N = net.layers.length;

    for (u32 i = 0; i < N; i++)
    {
        auto& layer = net.layers.data[N - 1 - i];
        update_back(layer);
    }
}


