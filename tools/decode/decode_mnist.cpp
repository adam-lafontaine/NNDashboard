#include "../../libs/util/memory_buffer.hpp"

#include <fstream>
#include <cstdio>
#include <filesystem>

namespace mb = memory_buffer;
namespace fs = std::filesystem;


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1


class ImageData
{
public:
    bool ok = false;

    u32 image_count = 0;

    u32 image_width = 0;
    u32 image_height = 0;

    MemoryBuffer<u8> pixel_buffer;
};


class LabelData
{
public:
    bool ok = false;

    u32 label_count = 0;

    MemoryBuffer<u8> label_buffer;
};


static void print_data_props(ImageData const& data, cstr msg)
{
    printf("Image data (%s):\n", msg);
    printf("%s\n", (data.ok ? "OK" : "ERROR"));
    if (!data.ok)
    {
        return;
    }

    printf(" image count: %u\n", data.image_count);
    printf(" image width: %u\n", data.image_width);
    printf("image height: %u\n", data.image_height);
}


static void print_data_props(LabelData const& data, cstr msg)
{
    printf("Label data (%s):\n", msg);
    printf("%s\n", (data.ok ? "OK" : "ERROR"));
    if (!data.ok)
    {
        return;
    }

    printf("label count: %u\n", data.label_count);
}


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


static ImageData load_image_data(fs::path const& filepath)
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

    MemoryBuffer<u8> buffer;

    mb::create_buffer<u8>(buffer, n_bytes);
    if (!buffer.ok)
    {
        file.close();
        return data;
    }

    file.read((char*)buffer.data_, n_bytes);

    file.close();

    data.image_count = n_images;
    data.image_width = n_cols;
    data.image_height = n_rows;
    data.pixel_buffer = buffer;
    data.ok = true;

    return data;
}


static LabelData load_label_data(fs::path const& filepath)
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

    MemoryBuffer<u8> buffer;

    mb::create_buffer<u8>(buffer, n_bytes);
    if (!buffer.ok)
    {
        file.close();
        return data;
    }

    file.read((char*)buffer.data_, n_bytes);

    file.close();

    data.label_count = n_labels;
    data.label_buffer = buffer;
    data.ok = true;

    return data;
}


int main()
{
    auto image_train = "train-images.idx3-ubyte";
    auto image_test = "t10k-images.idx3-ubyte";
    auto label_train = "train-labels.idx1-ubyte";
    auto label_test = "t10k-labels.idx1-ubyte";

    auto root = "C:\\D_Data\\Repos\\NNDashboard";

    auto dir = fs::path(root) / "resources/test_data";

    auto image_train_data = load_image_data(dir / image_train);
    auto image_test_data = load_image_data(dir / image_test);
    auto label_train_data = load_label_data(dir / label_train);
    auto label_test_data = load_label_data(dir / label_test);

    print_data_props(image_train_data, "image train");
    print_data_props(image_test_data, "image test");
    print_data_props(label_train_data, "label train");
    print_data_props(label_test_data, "label test");

    mb::destroy_buffer(image_train_data.pixel_buffer);
    mb::destroy_buffer(image_test_data.pixel_buffer);
    mb::destroy_buffer(label_train_data.label_buffer);
    mb::destroy_buffer(label_test_data.label_buffer);

    return EXIT_SUCCESS;
}

#include "../../libs/alloc_type/alloc_type.cpp"