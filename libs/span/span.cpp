#pragma once

#include "span.hpp"

#ifdef __AVX__
#define SPAN_SIMD_128
#endif

#ifdef __AVX2__
#define SPAN_SIMD_128
#define SPAN_SIMD_256
#endif



#ifdef SPAN_SIMD_128
#include <immintrin.h>

/* defines */

namespace span
{
    using i128 = __m128i;
    using f128 = __m128;

#ifdef SPAN_SIMD_256

    using i256 = __m256i;
    using f256 = __m256;

#endif
}

#endif


/* defines */

namespace span
{
    constexpr auto size32 = sizeof(u32);
    constexpr auto size64 = sizeof(u64);
    constexpr auto size128 = 2 * size64;
    constexpr auto size256 = 2 * size128;
    constexpr auto size512 = 2 * size256;
    constexpr auto size1024 = 2 * size512;
    
}


/* bcopy */

namespace span
{
    static void bcopy_64(u8* src, u8* dst, u32 n_u8)
    {
        switch (n_u8)
        {
        case 1:
            *dst = *src;
            return;

        case 2:
            *(u16*)dst = *(u16*)src;
            return;

        case 3:
            *(u16*)dst = *(u16*)src;
            dst[2] = src[2];
            return;

        case 4:
            *(u32*)dst = *(u32*)src;
            return;
        
        case 5:
            *(u32*)dst = *(u32*)src;
            dst[4] = src[4];
            return;
        
        case 6:
            *(u32*)dst = *(u32*)src;
            *(u16*)(dst + 4) = *(u16*)(src + 4);
            return;
        
        case 7:
            *(u32*)dst = *(u32*)src;
            *(u16*)(dst + 4) = *(u16*)(src + 4);
            dst[6] = src[6];
            return;

        case 8:
            *(u64*)dst = *(u64*)src;
            return;

        default:
            break;
        }
    }


    static inline void bcopy_128(u8* src, u8* dst)
    {
        #ifdef SPAN_SIMD_128
        _mm_storeu_si128((i128*)dst, _mm_loadu_si128((i128*)src));
        #else
        ((u64*)dst)[0] = ((u64*)src)[0];
        ((u64*)dst)[1] = ((u64*)src)[1];
        #endif
    }


    static inline void bcopy_256(u8* src, u8* dst)
    {
        #ifdef SPAN_SIMD_256
        _mm256_storeu_si256((i256*)dst, _mm256_loadu_si256((i256*)src));
        #else
        bcopy_128(src, dst);
        bcopy_128(src + size128, dst + size128);
        #endif
    }
   

    static inline void bcopy_512(u8* src, u8* dst)
    {
        bcopy_256(src, dst);
        bcopy_256(src + size256, dst + size256);
    }


    static inline void bcopy_1024(u8* src, u8* dst)
    {
        bcopy_512(src, dst);
        bcopy_512(src + size512, dst + size512);
    }
}


/* copy_u8 */

namespace span
{
    static void copy_u8_64(u8* src, u8* dst, u64 len_u8)
    {
        auto const len64 = len_u8 / size64;
        auto const src64 = (u64*)src;
        auto const dst64 = (u64*)dst;

        for (u64 i = 0; i < len64; ++i)
        {
            dst64[i] = src64[i];
        }

        auto const len8 = len_u8 - len64 * size64;
        auto const src8 = (u8*)(src64 + len64);
        auto const dst8 = (u8*)(dst64 + len64);

        bcopy_64(src8, dst8, len8);
    }


    static void copy_u8_128(u8* src, u8* dst, u64 len_u8)
    {
        auto const n128 = len_u8 / size128;
        auto const end128 = n128 * size128;

        u64 i = 0;

        for (; i < end128; i += size128)
        {
            bcopy_128(src + i, dst + i);
        }

        i = len_u8 - size128;
        bcopy_128(src + i, dst + i);
    }


    static void copy_u8_256(u8* src, u8* dst, u64 len_u8)
    {
        auto const n256 = len_u8 / size256;
        auto const end256 = n256 * size256;

        u64 i = 0;

        for (; i < end256; i += size256)
        {            
            bcopy_256(src + i, dst + i);
        }

        i = len_u8 - size256;
        bcopy_256(src + i, dst + i);
    }


    static void copy_u8_512(u8* src, u8* dst, u64 len_u8)
    {
        auto const n512 = len_u8 / size512;
        auto const end512 = n512 * size512;

        u64 i = 0;

        for(; i < end512; i += size512)
        {
            bcopy_512(src + i, dst + i);
        }

        i = len_u8 - size512;
        bcopy_512(src + i, dst + i);
    }


    static void copy_u8_1024(u8* src, u8* dst, u64 len_u8)
    {
        auto const n1024 = len_u8 / size1024;
        auto const end1024 = n1024 * size1024;

        u64 i = 0;

        for(; i < end1024; i += size1024)
        {
            bcopy_1024(src + i, dst + i);
        }

        i = len_u8 - size1024;
        bcopy_1024(src + i, dst + i);
    }
}


/* bfill */

namespace span
{
#ifndef SPAN_SIMD_128

    static inline void bfill_u8_64(u8* dst, u8 value)
    {
        dst[0] = value;
        dst[1] = value;
        dst[2] = value;
        dst[3] = value;
        dst[4] = value;
        dst[5] = value;
        dst[6] = value;
        dst[7] = value;
    }

#endif


    static inline void bfill_u8_128(u8* dst, u8 value)
    {
        #ifdef SPAN_SIMD_128
        _mm_storeu_si128((i128*)dst, _mm_set1_epi8(value));
        #else
        bfill_u8_64(dst, value);
        bfill_u8_64(dst + size64, value);
        #endif
    }


    static inline void bfill_u8_256(u8* dst, u8 value)
    {
        #ifdef SPAN_SIMD_256
        _mm256_storeu_si256((i256*)dst, _mm256_set1_epi8(value));
        #else
        bfill_u8_128(dst, value);
        bfill_u8_128(dst + size128, value);
        #endif
    }


    static inline void bfill_u8_512(u8* dst, u8 value)
    {
        bfill_u8_256(dst, value);
        bfill_u8_256(dst + size256, value);
    }


    static inline void bfill_u8_1024(u8* dst, u8 value)
    {
        bfill_u8_512(dst, value);
        bfill_u8_512(dst + size512, value);
    }


    static inline void bfill_i32_128(u8* dst, i32 value)
    {
        #ifdef SPAN_SIMD_128
        _mm_storeu_si128((i128*)dst, _mm_set1_epi32(value));
        #else
        ((i32*)dst)[0] = value;
        ((i32*)dst)[1] = value;
        ((i32*)dst)[2] = value;
        ((i32*)dst)[3] = value;
        #endif
    }


    static inline void bfill_i32_256(u8* dst, i32 value)
    {
        #ifdef SPAN_SIMD_256
        _mm256_storeu_si256((i256*)dst, _mm256_set1_epi32(value));
        #else
        bfill_i32_128(dst, value);
        bfill_i32_128(dst + size128, value);
        #endif
    }


    static inline void bfill_i32_512(u8* dst, i32 value)
    {
        
        bfill_i32_256(dst, value);
        bfill_i32_256(dst + size256, value);
    }


    static inline void bfill_i32_1024(u8* dst, u32 value)
    {
        bfill_i32_512(dst, value);
        bfill_i32_512(dst + size512, value);
    }
    
}


/* fill_u8 */

namespace span
{
    static void fill_u8_8(u8* dst, u8 value, u64 len_u8)
    {
        for (u32 i = 0; i < len_u8; i++)
        {
            dst[i] = value;
        }
    }


    static void fill_u8_128(u8* dst, u8 value, u64 len_u8)
    {
        auto const n128 = len_u8 / size128;
        auto const end128 = n128 * size128;

        u64 i = 0;

        for (; i < end128; i += size128)
        {
            bfill_u8_128(dst + i, value);
        }

        i = len_u8 - size128;
        bfill_u8_128(dst + i, value);
    }    


    static void fill_u8_256(u8* dst, u8 value, u64 len_u8)
    {
        auto const n256 = len_u8 / size256;
        auto const end256 = n256 * size256;

        u64 i = 0;

        for (; i < end256; i += size256)
        {
            bfill_u8_256(dst + i, value);
        }

        i = len_u8 - size256;
        bfill_u8_256(dst + i, value);
    }


    static void fill_u8_512(u8* dst, u8 value, u64 len_u8)
    {
        auto const n512 = len_u8 / size512;
        auto const end512 = n512 * size512;

        u64 i = 0;

        for (; i < end512; i += size512)
        {
            bfill_u8_512(dst + i, value);
        }

        i = len_u8 - size512;
        bfill_u8_512(dst + i, value);
    }


    static void fill_u8_1024(u8* dst, u8 value, u64 len_u8)
    {
        auto const n1024 = len_u8 / size1024;
        auto const end1024 = n1024 * size1024;

        u64 i = 0;

        for (; i < end1024; i += size1024)
        {
            bfill_u8_1024(dst + i, value);
        }

        i = len_u8 - size1024;
        bfill_u8_1024(dst + i, value);
    }
}


/* fill_u32 */

namespace span
{
    static void fill_u32_32(u32* dst, u32 value, u64 len_u32)
    {
        for (u32 i = 0; i < len_u32; i++)
        {
            dst[i] = value;
        }
    }


    static void fill_u32_128(u32* dst, u32 value, u64 len_u32)
    {
        auto const ival = *((int*)(&value));

        auto const len_u8 = len_u32 * size32;
        auto const n128 = len_u8 / size128;
        auto const end128 = n128 * size128;        

        u8* d8 = (u8*)dst;

        u64 i = 0;

        for (; i < end128; i += size128)
        {
            bfill_i32_128(d8 + i, ival);
        }

        i = len_u8 - size128;
        bfill_i32_128(d8 + i, ival);
    }


    static void fill_u32_256(u32* dst, u32 value, u64 len_u32)
    {
        auto const ival = *((int*)(&value));

        auto const len_u8 = len_u32 * size32;
        auto const n256 = len_u8 / size256;
        auto const end256 = n256 * size256;

        u8* d8 = (u8*)dst;

        u64 i = 0;

        for (; i < end256; i += size256)
        {
            bfill_i32_256(d8 + i, ival);
        }

        i = len_u8 - size256;
        bfill_i32_256(d8 + i, ival);
    }


    static void fill_u32_512(u32* dst, u32 value, u64 len_u32)
    {
        auto const ival = *((int*)(&value));

        auto const len_u8 = len_u32 * size32;
        auto const n512 = len_u8 / size512;
        auto const end512 = n512 * size512;

        u8* d8 = (u8*)dst;

        u64 i = 0;

        for (; i < end512; i += size512)
        {
            bfill_i32_512(d8 + i, ival);
        }

        i = len_u8 - size512;
        bfill_i32_512(d8 + i, ival);
    }


    static void fill_u32_1024(u32* dst, u32 value, u64 len_u32)
    {
        auto const ival = *((int*)(&value));

        auto const len_u8 = len_u32 * size32;
        auto const n1024 = len_u8 / size1024;
        auto const end1024 = n1024 * size1024;

        u8* d8 = (u8*)dst;

        u64 i = 0;

        for (; i < end1024; i += size1024)
        {
            bfill_i32_1024(d8 + i, ival);
        }

        i = len_u8 - size1024;
        bfill_i32_1024(d8 + i, ival);
    }

}


/* add */

namespace span
{
    static void add_32(f32* a, f32* b, f32* dst, u32 len)
    {
        for (u32 i = 0; i < len; i++)
        {
            dst[i] = a[i] + b[i];
        }
    }


    static void add_128(f32* a, f32* b, f32* dst, u32 len)
    {
        #ifdef SPAN_SIMD_128

        constexpr u32 N = 4;
        u32 L = len - (len % N);

        u32 i = 0;
        for (i = 0; i < L; i += N)
        {
            f128 va = _mm_loadu_ps(a + i);
            f128 vb = _mm_loadu_ps(b + i);

            _mm_storeu_ps(dst + i, _mm_add_ps(va, vb));
        }

        add_32(a + i, b + i, dst + i, len - i);

        #else

        add_32(a, b, dst, len);

        #endif
    }


    static void add_256(f32* a, f32* b, f32* dst, u32 len)
    {
        #ifdef SPAN_SIMD_256

        constexpr u32 N = 8;
        u32 L = len - (len % N);

        u32 i = 0;
        for (i = 0; i < L; i += N)
        {
            f256 va = _mm256_loadu_ps(a + i);
            f256 vb = _mm256_loadu_ps(b + i);

            _mm256_storeu_ps(dst + i, _mm256_add_ps(va, vb));
        }

        add_32(a + i, b + i, dst + i, len - i);

        #else

        add_128(a, b, dst, len);

        #endif
    }
}


/* sub */

namespace span
{
    static void sub_32(f32* a, f32* b, f32* dst, u32 len)
    {
        for (u32 i = 0; i < len; i++)
        {
            dst[i] = a[i] - b[i];
        }
    }


    static void sub_128(f32* a, f32* b, f32* dst, u32 len)
    {
        #ifdef SPAN_SIMD_128

        constexpr u32 N = 4;
        u32 L = len - (len % N);

        u32 i = 0;
        for (i = 0; i < L; i += N)
        {
            f128 va = _mm_loadu_ps(a + i);
            f128 vb = _mm_loadu_ps(b + i);

            _mm_storeu_ps(dst + i, _mm_sub_ps(va, vb));
        }

        sub_32(a + i, b + i, dst + i, len - i);


        #else

        sub_32(a, b, dst, len);

        #endif
    }


    static void sub_256(f32* a, f32* b, f32* dst, u32 len)
    {
        #ifdef SPAN_SIMD_256

        constexpr u32 N = 8;
        u32 L = len - (len % N);

        u32 i = 0;
        for (i = 0; i < L; i += N)
        {
            f256 va = _mm256_loadu_ps(a + i);
            f256 vb = _mm256_loadu_ps(b + i);

            _mm256_storeu_ps(dst + i, _mm256_sub_ps(va, vb));
        }

        sub_32(a + i, b + i, dst + i, len - i);

        #else

        sub_128(a, b, dst, len);

        #endif
    }
}


/* dot */

namespace span
{
    static f32 dot_32(f32* a, f32* b, u32 len)
    {
        f32 res = 0.0f;
        for (u32 i = 0; i < len; i++)
        {
            res += a[i] * b[i];
        }

        return res;
    }


    static f32 dot_128(f32* a, f32* b, u32 len)
    {
        #ifdef SPAN_SIMD_128

        constexpr u32 N = 4;
        u32 L = len - (len % N);

        f128 vsum = _mm_setzero_ps();
        alignas(N * sizeof(f32)) f32 res[N];

        u32 i = 0;
        for (i = 0; i < L; i += N)
        {
            f128 va = _mm_loadu_ps(a + i);
            f128 vb = _mm_loadu_ps(b + i);
            f128 vm = _mm_mul_ps(va, vb);
            vsum = _mm_add_ps(vsum, vm);
        }

        _mm_storeu_ps(res, vsum);
        f32 sum = res[0] + res[1] + res[2] + res[3];

        sum += dot_32(a + i, b + i, len - i);

        return sum;

        #else

        return dot_32(a, b, len);

        #endif
    }


    static f32 dot_256(f32* a, f32* b, u32 len)
    {
        #ifdef SPAN_SIMD_256
        
        constexpr u32 N = 8;
        u32 L = len - (len % N);

        f256 vsum = _mm256_setzero_ps();
        alignas(N * sizeof(f32)) f32 res[N];

        u32 i = 0;
        for (i = 0; i < L; i += N)
        {
            f256 va = _mm256_loadu_ps(a + i);
            f256 vb = _mm256_loadu_ps(b + i);
            f256 vm = _mm256_mul_ps(va, vb);
            vsum = _mm256_add_ps(vsum, vm);
        }

        _mm256_storeu_ps(res, vsum);
        f32 sum = res[0] + res[1] + res[2] + res[3] +
            res[4] + res[5] + res[6] + res[7];

        sum += dot_32(a + i, b + i, len - i);

        return sum;

        #else

        return dot_128(a, b, len);

        #endif
    }
}


/* api */

namespace span
{
    void copy_u8(u8* src, u8* dst, u64 len_u8)
    {
        auto const n_64 = len_u8 / 8;

        switch (n_64)
        {
        case 0:
        case 1:
            copy_u8_64(src, dst, len_u8);
            break;
        case 2:
        case 3:
            copy_u8_128(src, dst, len_u8);
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            copy_u8_256(src, dst, len_u8);
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            copy_u8_512(src, dst, len_u8);
            break;
        default:
            copy_u8_1024(src, dst, len_u8);
        }
    }


    void fill_u8(u8* dst, u8 value, u64 len_u8)
    {
        auto const n_64 = len_u8 / 8;

        switch (n_64)
        {
        case 0:
        case 1:
            fill_u8_8(dst, value, len_u8);
            break;
        case 2:
        case 3:
            fill_u8_128(dst, value, len_u8);
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            fill_u8_256(dst, value, len_u8);
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            fill_u8_512(dst, value, len_u8);
            break;
        default:
            fill_u8_1024(dst, value, len_u8);
        }
    }


    void fill_u32(u32* dst, u32 value, u64 len_u32)
    {
        auto const n_64 = len_u32 / 2;

        switch (n_64)
        {
        case 0:
        case 1:
            fill_u32_32(dst, value, len_u32);
            break;
        case 2:
        case 3:
            fill_u32_128(dst, value, len_u32);
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            fill_u32_256(dst, value, len_u32);
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            fill_u32_512(dst, value, len_u32);
            break;
        default:
            fill_u32_1024(dst, value, len_u32);
        }
    }
}


namespace span
{
    void add(SpanView<f32> const& a, SpanView<f32> const& b, SpanView<f32> const& dst)
    {
        auto len = a.length; // == b.length == dst.length

        switch (len)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            add_32(a.data, b.data, dst.data, len);
            break;

        case 4:
        case 5:
        case 6:
        case 7:
            add_128(a.data, b.data, dst.data, len);
            break;

        default: 
            add_256(a.data, b.data, dst.data, len);
            break;
        }
    }
    

    void sub(SpanView<f32> const& a, SpanView<f32> const& b, SpanView<f32> const& dst)
    {
        auto len = a.length; // == b.length == dst.length

        switch (len)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            sub_32(a.data, b.data, dst.data, len);
            break;

        case 4:
        case 5:
        case 6:
        case 7:
            sub_128(a.data, b.data, dst.data, len);
            break;

        default: 
            sub_256(a.data, b.data, dst.data, len);
            break;
        }
    }
    

    f32 dot(SpanView<f32> const& a, SpanView<f32> const& b)
    {
        auto len = a.length; // == b.length

        switch (len)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            return dot_32(a.data, b.data, len);
        case 4:
        case 5:
        case 6:
        case 7:
            return dot_128(a.data, b.data, len);

        default: 
            return dot_256(a.data, b.data, len);
        }

        f32 res = 0.0f;
        for (u32 i = 0; i < len; i++)
        {
            res += a.data[i] * b.data[i];
        }

        return res;
    }
}