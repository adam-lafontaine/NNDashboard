#pragma once

#include "../util/memory_buffer.hpp"

namespace mb = memory_buffer;


/*  image basic */

namespace image
{
    class RGBAu8
    {
    public:
        u8 red;
        u8 green;
        u8 blue;
        u8 alpha;
    };

    using Pixel = RGBAu8;
    using Image = Matrix2D<Pixel>;
    using ImageView = MatrixView2D<Pixel>;
    using ImageGray = Matrix2D<u8>;
    using GrayView = MatrixView2D<u8>;


    bool create_image(Image& image, u32 width, u32 height);

    void destroy_image(Image& image);


    inline u32 as_u32(Pixel p)
    {
        return  *((u32*)(&p));
    }


    inline Image as_image(ImageView const& view)
    {
        Image image;
        image.width = view.width;
        image.height = view.height;
        image.data_ = view.matrix_data_;

        return image;
    }
}


namespace image
{
    using Buffer8 = MemoryBuffer<u8>;
    using Buffer32 = MemoryBuffer<Pixel>;


    constexpr inline Pixel to_pixel(u8 red, u8 green, u8 blue, u8 alpha)
    {
        Pixel p{};
        p.red = red;
        p.green = green;
        p.blue = blue;
        p.alpha = alpha;

        return p;
    }


    constexpr inline Pixel to_pixel(u8 red, u8 green, u8 blue)
    {
        return to_pixel(red, green, blue, 255);
    }


    constexpr inline Pixel to_pixel(u8 gray)
    {
        return to_pixel(gray, gray, gray);
    } 


    inline Buffer8 create_buffer8(u32 n_pixels)
	{
		Buffer8 buffer;
		mb::create_buffer(buffer, n_pixels);
		return buffer;
	}


    inline Buffer32 create_buffer32(u32 n_pixels)
	{
		Buffer32 buffer;
		mb::create_buffer(buffer, n_pixels);
		return buffer;
	}


    inline Buffer8 create_buffer8(u32 n_pixels, cstr tag)
	{
		Buffer8 buffer;
		mb::create_buffer(buffer, n_pixels, tag);
		return buffer;
	}


    inline Buffer32 create_buffer32(u32 n_pixels, cstr tag)
	{
		Buffer32 buffer;
		mb::create_buffer(buffer, n_pixels, tag);
		return buffer;
	}


    inline Rect2Du32 make_rect(u32 width, u32 height)
    {
        Rect2Du32 range{};
        range.x_begin = 0;
        range.x_end = width;
        range.y_begin = 0;
        range.y_end = height;

        return range;
    }


    inline Rect2Du32 make_rect(u32 x_begin, u32 y_begin, u32 width, u32 height)
    {
        Rect2Du32 range{};
        range.x_begin = x_begin;
        range.x_end = x_begin + width;
        range.y_begin = y_begin;
        range.y_end = y_begin + height;

        return range;
    }
}


/* make_view */

namespace image
{
    ImageView make_view(Image const& image);

    ImageView make_view(u32 width, u32 height, Buffer32& buffer);

    GrayView make_view(u32 width, u32 height, Buffer8& buffer);
}