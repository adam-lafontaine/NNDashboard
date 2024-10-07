#pragma once

#include "mnist.hpp"

#include <fstream>

namespace mb = memory_buffer;

namespace mnist
{
    static u32 reverse_bytes(u32 val)
    {
        return ((val >> 24) & 0xff) |       // Move byte 3 to byte 0
            ((val << 8) & 0xff0000) |    // Move byte 1 to byte 2
            ((val >> 8) & 0xff00) |      // Move byte 2 to byte 1
            ((val << 24) & 0xff000000);  // Move byte 0 to byte 3
    }


    u32 read_u32(std::ifstream& file)
    {
        u32 val;
        auto dst = (char*)(&val);

        file.read(dst, sizeof(val));

        return reverse_bytes(val);
    }
}


namespace mnist
{
    ImageData load_image_data(cstr filepath)
    {
        constexpr u32 CODE = 2051;

        ImageData data{};
        data.ok = false;

        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
        {
            return data;
        }

        auto code = read_u32(file);

        if (code != CODE)
        {
            return data;
        }

        auto n_images = read_u32(file);
        auto n_rows = read_u32(file);
        auto n_cols = read_u32(file);

        auto n_bytes = n_images * n_rows * n_cols;

        MemoryBuffer<u8> buffer8;

        mb::create_buffer<u8>(buffer8, n_bytes, "mnist data pixels");
        if (!buffer8.ok)
        {
            file.close();
            return data;
        }

        file.read((char*)buffer8.data_, n_bytes);

        file.close();

        MemoryBuffer<f32> buffer32;
        mb::create_buffer<f32>(buffer32, n_rows * n_cols, "mnist data input");
        if (!buffer32.ok)
        {
            mb::destroy_buffer(buffer8);
            return data;
        }

        data.image_count = n_images;
        data.image_width = n_cols;
        data.image_height = n_rows;
        data.pixel_buffer = buffer8;
        data.input_buffer = buffer32;
        data.ok = true;

        return data;
    }


    LabelData load_label_data(cstr filepath)
    {
        constexpr u32 CODE = 2049;

        LabelData data{};
        data.ok = false;

        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
        {
            return data;
        }

        auto code = read_u32(file);

        if (code != CODE)
        {
            return data;
        }

        auto n_labels = read_u32(file);

        auto n_bytes = n_labels;

        MemoryBuffer<u8> buffer8;
        mb::create_buffer<u8>(buffer8, n_bytes, "mnist labels");
        if (!buffer8.ok)
        {
            file.close();
            return data;
        }

        file.read((char*)buffer8.data_, n_bytes);

        file.close();

        MemoryBuffer<f32> buffer32;
        mb::create_buffer<f32>(buffer32, 10, "mnist output");
        if (!buffer32.ok)
        {
            mb::destroy_buffer(buffer8);
            return data;
        }

        data.label_count = n_labels;
        data.label_buffer = buffer8;
        data.output_buffer = buffer32;
        data.ok = true;

        return data;
    }


    void destroy_data(ImageData& data)
    {
        mb::destroy_buffer(data.pixel_buffer);
        mb::destroy_buffer(data.input_buffer);
    }


    void destroy_data(LabelData& data)
    {
        mb::destroy_buffer(data.label_buffer);
        mb::destroy_buffer(data.output_buffer);
    }


    SpanView<f32> data_at(ImageData const& data, u32 index)
    {
        auto len = data.image_width * data.image_height;

        auto begin = data.pixel_buffer.data_ + index * len;

        constexpr f32 F = 1.0f / 255.0f;

        auto span = span::make_view(data.input_buffer);

        for (u32 i = 0; i < len; i++)
        {
            span.data[i] = F * begin[i];
        }

        return span;
    }


    SpanView<f32> data_at(LabelData const& data, u32 index)
    {
        auto span = span::make_view(data.output_buffer);

        auto label = data.label_buffer.data_[index];
        
        for (u8 i = 0; i < 10; i++)
        {
            span.data[i] = label == 8 ? 1.0f : 0.0f;
        }

        return span;
    }


    img::GrayView image_at(ImageData const& data, u32 index)
    {
        auto offset = index * data.image_width * data.image_height;

        img::GrayView view{};
        view.width = data.image_width;
        view.height = data.image_height;
        view.matrix_data_ = data.pixel_buffer.data_ + offset;

        return view;
    }


    u8 label_at(LabelData const& data, u32 index)
    {
        return data.label_buffer.data_[index];
    }
}