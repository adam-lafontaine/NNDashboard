#pragma once

#include "../image/image.hpp"
#include "../span/span.hpp"

namespace img = image;


namespace mnist
{
    class ImageData
    {
    public:
        bool ok = false;

        u32 image_count = 0;

        u32 image_width = 0;
        u32 image_height = 0;

        MemoryBuffer<u8> pixel_buffer;
        MemoryBuffer<f32> input_buffer;
    };


    class LabelData
    {
    public:
        bool ok = false;

        u32 label_count = 0;

        MemoryBuffer<u8> label_buffer;
        MemoryBuffer<f32> output_buffer;
    };
}


namespace mnist
{
    ImageData load_image_data(cstr filepath);

    LabelData load_label_data(cstr filepath);


    void destroy_data(ImageData& data);

    void destroy_data(LabelData& data);


    SpanView<f32> input_at(ImageData const& data, u32 index);

    SpanView<f32> output_at(LabelData const& data, u32 index);


    img::GrayView image_at(ImageData const& data, u32 index);

    u8 label_at(LabelData const& data, u32 index);
}