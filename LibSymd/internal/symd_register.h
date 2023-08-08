#pragma once
#include <type_traits>
#include <limits>
#include <array>
#include "../bfloat16.h"


namespace symd
{
    namespace __internal__
    {
        // Platform check macros
        // https://abseil.io/docs/cpp/platforms/macros#architecture

    #if defined(_M_X64) || defined(_M_IX86) || defined(__x86_64__) || defined(__i386__)
    #define SYMD_SSE 100
    #include <immintrin.h>
        constexpr int SYMD_LEN = 8;
        constexpr bool Is_SSE = true;
        constexpr bool Is_NEON = false;
    #elif defined(_M_ARM64) || defined(_M_ARM) || defined(__aarch64__) || defined (__arm__)
    #define SYMD_NEON 100
    #include <arm_neon.h>
        constexpr int SYMD_LEN = 4;
        constexpr bool Is_SSE = false;
        constexpr bool Is_NEON = true;
    #endif

        template <typename T>
        class UnderlyingRegister
        {
        public:

            constexpr static bool is_supported_type()
            {
                return false;
            }
        };


        template <>
        class UnderlyingRegister<float>
        {
        public:

    #ifdef SYMD_SSE
            using Type = __m256;
    #elif defined SYMD_NEON
            using Type = float32x4_t;
    #endif

            using DType = float;

            constexpr static bool is_supported_type()
            {
                return true;
            }
        };

        template <>
        class UnderlyingRegister<symd::bfloat16>
        {
        public:

    #ifdef SYMD_SSE
            using Type = __m256;
    #elif defined SYMD_NEON
            using Type = float32x4_t;
    #endif

            using DType = float;

            constexpr static bool is_supported_type()
            {
                return true;
            }
        };

        template <>
        class UnderlyingRegister<int>
        {
        public:

    #ifdef SYMD_SSE
            using Type = __m256i;
    #elif defined SYMD_NEON
            using Type = int32x4_t; // Neon has 4 elements
    #endif

            using DType = int;

            constexpr static bool is_supported_type()
            {
                return true;
            }
        };

        template <>
        class UnderlyingRegister<unsigned char>
        {
        public:

    #ifdef SYMD_SSE
            using Type = __m128i;
    #elif defined SYMD_NEON
            using Type = uint8x8_t; // Neon has 4 elements
    #endif

            using DType = unsigned char;

            constexpr static bool is_supported_type()
            {
                return true;
            }
        };


        template <>
        class UnderlyingRegister<double>
        {
        public:

    #ifdef SYMD_SSE
            using Type = std::array<__m256d, 2>;
    #elif defined SYMD_NEON
            using Type = std::array<float64x2_t, 2>; // Neon has 4 elements
    #endif

            using DType = double;

            constexpr static bool is_supported_type()
            {
                return true;
            }
        };


        template <typename T>
        constexpr void assert_supported_type()
        {
            static_assert(UnderlyingRegister<T>::is_supported_type(), "Unsuported Symd type");
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // SymdRegister class - core class for SIMD registers
        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T>
        class SymdRegister
        {
        public:

            union
            {
                typename UnderlyingRegister<T>::Type _reg;
                typename UnderlyingRegister<T>::DType _ptrToData[SYMD_LEN];
            };

            // Constructs uninitialized register
            SymdRegister()
            {
                assert_supported_type<T>();
            }

            // Constructs regitesr with provided SSE or NEON registers
            SymdRegister(typename UnderlyingRegister<T>::Type other)
            {
                assert_supported_type<T>();

                if constexpr (std::is_same_v<T, double>)
                {
                    _reg[0] = other[0];
                    _reg[1] = other[1];
                }
                else
                {
                    _reg = other;
                }
            }

            // Reads 8 floats from memory and constructs register
            SymdRegister(const T* ptr)
            {
                assert_supported_type<T>();

                if constexpr (std::is_same_v<T, float>)
                {
    #ifdef SYMD_SSE
                    _reg = _mm256_loadu_ps(ptr);
    #elif defined SYMD_NEON
                    _reg = vld1q_f32(ptr);
    #endif
                }
                else if constexpr (std::is_same_v<T, symd::bfloat16>)
                {
                    
    #ifdef SYMD_SSE
                    __m128i eight_shorts = _mm_loadu_si128((__m128i*)ptr);
                    __m128i zeros = _mm_setzero_si128();

                    __m128i lo = _mm_unpacklo_epi16(zeros, eight_shorts);
                    __m128i hi = _mm_unpackhi_epi16(zeros, eight_shorts);

                    _reg = _mm256_castsi256_ps(_mm256_set_m128i(hi, lo));

    #elif defined SYMD_NEON
                    assert(false);
                    // std::array<float, 4> data4 = symd::bfloat16::load_4(ptr);
                    // _reg = vld1q_f32(data4.data());
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    _reg = _mm256_loadu_si256((__m256i*)ptr);
    #elif defined SYMD_NEON
                    _reg = vld1q_s32(ptr);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    _reg = _mm_loadu_si64((__m128i*)ptr);
    #elif defined SYMD_NEON
                    _reg = vld1q_s32(ptr);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
    #ifdef SYMD_SSE
                    _reg[0] = _mm256_loadu_pd(ptr + 0);
                    _reg[1] = _mm256_loadu_pd(ptr + 4);
    #elif defined SYMD_NEON
                    _reg[0] = vld1q_f64(ptr + 0);
                    _reg[1] = vld1q_f64(ptr + 2);
    #endif
                }
            }

            // Costructs register with all elements equal to other
            SymdRegister(T other)
            {
                assert_supported_type<T>();

                if constexpr (std::is_same_v<T, float>)
                {
    #ifdef SYMD_SSE
                    _reg = _mm256_set1_ps(other);
    #elif defined SYMD_NEON
                    _reg = vdupq_n_f32(other);
    #endif
                }
                else if constexpr (std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    _reg = _mm256_set1_ps((float)other);
    #elif defined SYMD_NEON
                    _reg = vld1q_f32((float)other);
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    _reg = _mm256_set1_epi32(other);
    #elif defined SYMD_NEON
                    _reg = vdupq_n_s32(x);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    _reg = _mm_set1_epi8(other);
    #elif defined SYMD_NEON
                    _reg = vdup_n_u8(other);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
    #ifdef SYMD_SSE
                    _reg[0] = _mm256_set1_pd(other);

    #elif defined SYMD_NEON
                    _reg[0] = vdupq_n_f64(other);
    #endif
                    _reg[1] = _reg[0];
                }
            }

            /////////////////////////////////////////////////////////////////////////////////////
            // Arithmetic operators
            /////////////////////////////////////////////////////////////////////////////////////

            SymdRegister operator+(const SymdRegister<T>& other) const
            {
                assert_supported_type<T>();

                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_add_ps(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vaddq_f32(_reg, other._reg),
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_add_epi32(_reg, other._reg);
    #elif defined SYMD_NEON
                    return SymdRegister(vaddq_s32(_reg, other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    return _mm_adds_epu8(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vadd_u8(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return SymdRegister(typename UnderlyingRegister<T>::Type {
    #ifdef SYMD_SSE
                        _mm256_add_pd(_reg[0], other._reg[0]),
                        _mm256_add_pd(_reg[1], other._reg[1])
    #elif defined SYMD_NEON
                        vaddq_f64(_reg[0], other._reg[0]),
                        vaddq_f64(_reg[1], other._reg[1])
    #endif
                    });
                }
            }

            SymdRegister operator-(const SymdRegister<T>& other) const
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_sub_ps(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vsubq_f32(_reg, other._reg),
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_sub_epi32(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vsubq_s32(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    return _mm_subs_epu8(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vsub_u8(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type {
    #ifdef SYMD_SSE
                        _mm256_sub_pd(_reg[0], other._reg[0]),
                        _mm256_sub_pd(_reg[1], other._reg[1])
    #elif defined SYMD_NEON
                        vsubq_f64(_reg[0], other._reg[0]),
                        vsubq_f64(_reg[1], other._reg[1])
    #endif
                    };
                }
            }

            SymdRegister operator*(const SymdRegister<T>& other) const
            {
                static_assert(!std::is_same_v<T, unsigned char>, "Multiplication not supported for unsigned char. Convert to other type.");

                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_mul_ps(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vmulq_f32(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_mullo_epi32(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vmulq_s32(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                        _mm256_mul_pd(_reg[0], other._reg[0]),
                        _mm256_mul_pd(_reg[1], other._reg[1])
    #elif defined SYMD_NEON
                        vmulq_f64(_reg[0], other._reg[0]),
                        vmulq_f64(_reg[1], other._reg[1])
    #endif
                    };
                }
            }

            SymdRegister operator/(const SymdRegister<T>& other) const
            {
                static_assert(!std::is_same_v<T, int>, "Division not supported for int. Convert to other type.");
                static_assert(!std::is_same_v<T, unsigned char>, "Division not supported for unsigned char. Convert to other type.");

                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_div_ps(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vdivq_f32(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                        _mm256_div_pd(_reg[0], other._reg[0]),
                        _mm256_div_pd(_reg[1], other._reg[1])
    #elif defined SYMD_NEON
                        vdivq_f64(_reg[0], other._reg[0]),
                        vdivq_f64(_reg[1], other._reg[1])
    #endif
                    };
                }
            }

            SymdRegister operator&(const SymdRegister<T>& other) const
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_and_ps(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vreinterpretq_f32_u32(vandq_u32((uint32x4_t)_reg, (uint32x4_t)other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_and_si256(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vandq_s32(_reg, other._reg),
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    return _mm_and_si128(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vand_u8(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                        _mm256_and_pd(_reg[0], other._reg[0]),
                        _mm256_and_pd(_reg[1], other._reg[1])
    #elif defined SYMD_NEON
                        vreinterpretq_f64_u64(vandq_u64(vreinterpretq_u64_f64(_reg[0]), vreinterpretq_u64_f64(second._reg[0]))),
                        vreinterpretq_f64_u64(vandq_u64(vreinterpretq_u64_f64(_reg[1]), vreinterpretq_u64_f64(second._reg[1])))
    #endif
                    };
                }
            }

            SymdRegister operator|(const SymdRegister<T>& other) const
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_or_ps(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vreinterpretq_f32_u32(vorrq_u32((uint32x4_t)_reg, (uint32x4_t)other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_or_si256(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vorrq_s32(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    return _mm_or_si128(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vorr_u8(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                        _mm256_or_pd(_reg[0], other._reg[0]),
                        _mm256_or_pd(_reg[1], other._reg[1])
    #elif defined SYMD_NEON
                        vreinterpretq_f64_u64(vorrq_u64(vreinterpretq_u64_f64(_reg[0]), vreinterpretq_u64_f64(second._reg[0]))),
                        vreinterpretq_f64_u64(vorrq_u64(vreinterpretq_u64_f64(_reg[1]), vreinterpretq_u64_f64(second._reg[1])))
    #endif
                    };
                }
            }

            SymdRegister operator^(const SymdRegister<T>& other) const
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_xor_ps(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vreinterpretq_f32_u32(vxorq_u32((uint32x4_t)_reg, (uint32x4_t)other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_xor_si256(_reg, other._reg);
    #elif defined SYMD_NEON
                    return veorq_s32(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    return _mm_xor_si128(_reg, other._reg);
    #elif defined SYMD_NEON
                    return veor_u8(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                        _mm256_xor_pd(_reg[0], other._reg[0]),
                        _mm256_xor_pd(_reg[1], other._reg[1])
    #elif defined SYMD_NEON
                        vreinterpretq_f64_u64(vxorq_u32(vreinterpretq_u64_f64(_reg[0]), vreinterpretq_u64_f64(second._reg[0]))),
                        vreinterpretq_f64_u64(vxorq_u32(vreinterpretq_u64_f64(_reg[1]), vreinterpretq_u64_f64(second._reg[1])))
    #endif
                    };
                }
            }

            SymdRegister operator~() const
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    auto mask = _mm256_castsi256_ps(_mm256_set1_epi32(-1));
                    return _mm256_andnot_ps(_reg, mask);
    #elif defined SYMD_NEON
                    return vreinterpretq_f32_u32(vmvnq_u32(vreinterpretq_u32_f32(_reg)));
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_andnot_si256(_reg, _mm256_set1_epi32(-1));
    #elif defined SYMD_NEON
                    return vmvnq_s32(_reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    return _mm_andnot_si128(_reg, _mm_set1_epi32(-1));
    #elif defined SYMD_NEON
                    return vmvn_u8(_data);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
    #ifdef SYMD_SSE
                    __m256d mask = _mm256_castsi256_pd(_mm256_set1_epi32(-1));

                    return typename UnderlyingRegister<T>::Type{
                        _mm256_andnot_pd(_reg[0], mask),
                        _mm256_andnot_pd(_reg[1], mask)
    #elif defined SYMD_NEON
                    typename UnderlyingRegister<T>::Type res = {
                        vreinterpretq_f64_u32(vmvnq_u32(vreinterpretq_u32_f64(_reg[0]))),
                        vreinterpretq_f64_u32(vmvnq_u32(vreinterpretq_u32_f64(_reg[1])))
    #endif
                    };
                }
            }

            // Shift packed 32-bit integers in a right by count while shifting in zeros, and store the results in dst.
            SymdRegister operator>>(int num_bits) const
            {
                static_assert(std::is_same_v<T, int>,
                   "Shift opperations only supported in int registers -> convert to int");

                if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_srli_epi32(_reg, num_bits);
    #elif defined SYMD_NEON
    #endif
                }
            }

            // Shift packed 32-bit integers in a left by imm8 while shifting in zeros, and store the results in dst.
            SymdRegister operator<<(int num_bits) const
            {
                static_assert(std::is_same_v<T, int>,
                   "Shift opperations only supported in int registers -> convert to int");

                if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_slli_epi32(_reg, num_bits);
    #elif defined SYMD_NEON
    #endif
                }
            }


            /////////////////////////////////////////////////////////////////////////////////////
            // Logical operators
            /////////////////////////////////////////////////////////////////////////////////////

            SymdRegister operator!()
            {
                return ~(*this);
            }


            /////////////////////////////////////////////////////////////////////////////////////
            // Comparisson operators
            /////////////////////////////////////////////////////////////////////////////////////

            SymdRegister operator==(const SymdRegister<T>& other) const
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_cmp_ps(_reg, other._reg, _CMP_EQ_OQ);
    #elif defined SYMD_NEON
                    return vreinterpretq_f32_u32(vceqq_f32(_reg, other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_cmpeq_epi32(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vreinterpretq_s32_u32(vceqq_s32(_reg, other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    return _mm_cmpeq_epi8(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vceq_u8(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                       _mm256_cmp_pd(_reg[0], other._reg[0], _CMP_EQ_OQ),
                       _mm256_cmp_pd(_reg[1], other._reg[1], _CMP_EQ_OQ)
    #elif defined SYMD_NEON
                        vreinterpretq_f64_u64(vceqq_f64(_reg[0], other._reg[0])),
                        vreinterpretq_f64_u64(vceqq_f64(_reg[1], other._reg[1]))
    #endif
                    };
                }
            }

            SymdRegister operator!=(const SymdRegister<T>& other) const
            {
    #if defined SYMD_NEON
                return !(*this == other);
    #endif

    #ifdef SYMD_SSE
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
                    return _mm256_cmp_ps(_reg, other._reg, _CMP_NEQ_OQ);
                }
                else if constexpr (std::is_same_v<T, int>)
                {
                    return !(*this == other);
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
                    return !(*this == other);
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type {
                       _mm256_cmp_pd(_reg[0], other._reg[0], _CMP_NEQ_OQ),
                       _mm256_cmp_pd(_reg[1], other._reg[1], _CMP_NEQ_OQ)
                    };
                }
    #endif
            }

            SymdRegister operator<(const SymdRegister<T>& other) const
            {
                static_assert(!std::is_same_v<T, unsigned char>, "Comparison not supported for unsigned char. Convert to other type.");

                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_cmp_ps(_reg, other._reg, _CMP_LT_OQ);
    #elif defined SYMD_NEON
                    return vreinterpretq_f32_u32(vcltq_f32(_reg, other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_cmpgt_epi32(other._reg, _reg);
    #elif defined SYMD_NEON
                    return vreinterpretq_s32_u32(vcltq_s32(_reg, other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                       _mm256_cmp_pd(_reg[0], other._reg[0], _CMP_LT_OQ),
                       _mm256_cmp_pd(_reg[1], other._reg[1], _CMP_LT_OQ)
    #elif defined SYMD_NEON
                        vreinterpretq_f64_u64(vcltq_f64(_reg[0], other._reg[0])),
                        vreinterpretq_f64_u64(vcltq_f64(_reg[1], other._reg[1]))
    #endif
                    };
                }
            }

            SymdRegister operator>(const SymdRegister<T>& other) const
            {
                static_assert(!std::is_same_v<T, unsigned char>, "Comparison not supported for unsigned char. Convert to other type.");

                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_cmp_ps(_reg, other._reg, _CMP_GT_OQ);
    #elif defined SYMD_NEON
                    return vreinterpretq_f32_u32(vcgtq_f32(_reg, other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_cmpgt_epi32(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vreinterpretq_s32_u32(vcgtq_s32(_reg, other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                       _mm256_cmp_pd(_reg[0], other._reg[0], _CMP_GT_OQ),
                       _mm256_cmp_pd(_reg[1], other._reg[1], _CMP_GT_OQ)
    #elif defined SYMD_NEON
                        vreinterpretq_f64_u64(vcgtq_f64(_reg[0], other._reg[0])),
                        vreinterpretq_f64_u64(vcgtq_f64(_reg[1], other._reg[1]))
    #endif
                    };
                }
            }

            SymdRegister operator<=(const SymdRegister<T>& other) const
            {
                static_assert(!std::is_same_v<T, unsigned char>, "Comparison not supported for unsigned char. Convert to other type.");

                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_cmp_ps(_reg, other._reg, _CMP_LE_OQ);
    #elif defined SYMD_NEON
                    return vreinterpretq_f32_u32(vcleq_f32(_reg, other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
                    return !(*this > other);
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                       _mm256_cmp_pd(_reg[0], other._reg[0], _CMP_LE_OQ),
                       _mm256_cmp_pd(_reg[1], other._reg[1], _CMP_LE_OQ)
    #elif defined SYMD_NEON
                        vreinterpretq_f64_u64(vcleq_f64(_reg[0], other._reg[0])),
                        vreinterpretq_f64_u64(vcleq_f64(_reg[1], other._reg[1]))
    #endif
                    };
                }
            }

            SymdRegister operator>=(const SymdRegister<T>& other) const
            {
                static_assert(!std::is_same_v<T, unsigned char>, "Comparison not supported for unsigned char. Convert to other type.");

                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_cmp_ps(_reg, other._reg, _CMP_GE_OQ);
    #elif defined SYMD_NEON
                    return vreinterpretq_f32_u32(vcgeq_f32(_reg, other._reg));
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
                    return !(*this < other);
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                       _mm256_cmp_pd(_reg[0], other._reg[0], _CMP_GE_OQ),
                       _mm256_cmp_pd(_reg[1], other._reg[1], _CMP_GE_OQ)
    #elif defined SYMD_NEON
                        vreinterpretq_f64_u64(vcgeq_f64(_reg[0], other._reg[0])),
                        vreinterpretq_f64_u64(vcgeq_f64(_reg[1], other._reg[1]))
    #endif
                    };
                }
            }


            /////////////////////////////////////////////////////////////////////////////////////
            // Various
            /////////////////////////////////////////////////////////////////////////////////////

            SymdRegister min(const SymdRegister<T>& other) const
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_min_ps(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vminq_f32(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_min_epi32(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vminq_s32(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    return _mm_min_epu8(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vmin_u8(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                       _mm256_min_pd(_reg[0], other._reg[0]),
                       _mm256_min_pd(_reg[1], other._reg[1])
    #elif defined SYMD_NEON
                        vminq_f64(_reg[0], other._reg[0]),
                        vminq_f64(_reg[1], other._reg[1])
    #endif
                    };
                }
            }

            SymdRegister max(const SymdRegister<T>& other) const
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_max_ps(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vmaxq_f32(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_max_epi32(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vmaxq_s32(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    return _mm_max_epu8(_reg, other._reg);
    #elif defined SYMD_NEON
                    return vmax_u8(_reg, other._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                       _mm256_max_pd(_reg[0], other._reg[0]),
                       _mm256_max_pd(_reg[1], other._reg[1])
    #elif defined SYMD_NEON
                        vmaxq_f64(_reg[0], other._reg[0]),
                        vmaxq_f64(_reg[1], other._reg[1])
    #endif
                    };
                }
            }

            // Gets exponnent part of fp number
            SymdRegister<int> fp_exp() const
            {
                static_assert(std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>,
                   "fp_exp is only supported for float and bfloat16 for now.");

                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    __m256i integer_repr = _mm256_castps_si256(_reg); // Cast to integer so we can use bit ops
                    integer_repr = _mm256_slli_epi32(integer_repr, 1); // Shift left by one bit to align exp...
                    integer_repr = _mm256_srli_epi32(integer_repr, 24); // Shift right by 24 bits to align exp to right...
                    integer_repr = _mm256_sub_epi32(integer_repr, _mm256_set1_epi32(127));
                    return integer_repr;

    #elif defined SYMD_NEON
                    static_assert(false, "fp_exp not implemented for neon.");
    #endif
                }
    //             else if constexpr (std::is_same_v<T, double>)
    //             {
    // #ifdef SYMD_SSE
    //                 static_assert(false, "fp_exp is not implemented for double");
    // #elif defined SYMD_NEON
    //                 static_assert(false, "fp_exp not implemented for neon.");
    // #endif
    //             }
            }

            // Extracts 2^exp from input , where exp is exponent of float value.
            SymdRegister<T> fp_2_pow_exp() const
            {
                static_assert(std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>,
                   "fp_2_pow_exp is only supported for float and bfloat16 for now.");

                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    __m256i integer_repr = _mm256_castps_si256(_reg); // Cast to integer so we can use bit ops
                    integer_repr = _mm256_srli_epi32(integer_repr, 23); // Shift right by 23 bits to align exp to right...
                    integer_repr = _mm256_slli_epi32(integer_repr, 23); // Shifts back to the left to get back the float number
                    return _mm256_castsi256_ps(integer_repr);

    #elif defined SYMD_NEON
                    static_assert(false, "fp_exp not implemented for neon.");
    #endif
                }
    //             else if constexpr (std::is_same_v<T, double>)
    //             {
    // #ifdef SYMD_SSE
    //                 static_assert(false, "fp_exp is not implemented for double");
    // #elif defined SYMD_NEON
    //                     static_assert(false, "fp_exp not implemented for neon.");
    // #endif
    //             }
            }

            // Calculates 2^n with bit ops. By setting exp to float in bits.
            static SymdRegister<T> fastpow2(const SymdRegister<int>& n)
            {
                static_assert(
                    std::is_same_v<T, float> || 
                    std::is_same_v<T, symd::bfloat16> ||
                    std::is_same_v<T, double>,
                   "fastpow2 is only supported for floating point type.");

                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
                    SymdRegister<int> integer_exp = (n + 127) << 23;
                    return _mm256_castsi256_ps(integer_exp._reg);
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    // https://en.wikipedia.org/wiki/Double-precision_floating-point_format
                    SymdRegister<int> integer_exp = n + 1023;

                    __m256i lo_64 = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(integer_exp._reg, 0));
                    __m256i hi_64 = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(integer_exp._reg, 1));

                    lo_64 = _mm256_slli_epi64(lo_64, 52); // Shift to right by 52 to allign exp to right place in double format
                    hi_64 = _mm256_slli_epi64(hi_64, 52); // Shift to right by 52 to allign exp to right place in double format

                    return typename UnderlyingRegister<T>::Type{
                        _mm256_castsi256_pd(lo_64),
                        _mm256_castsi256_pd(hi_64)
                    };
                }
            }


            SymdRegister blend(const SymdRegister<T>& first, const SymdRegister<T>& sec) const
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return _mm256_blendv_ps(sec._reg, first._reg, _reg);
    #elif defined SYMD_NEON
                    return vbslq_f32(vreinterpretq_u32_f32(_reg), first._reg, sec._reg),
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_blendv_epi8(sec._reg, first._reg, _reg);
    #elif defined SYMD_NEON
                    return vbslq_s32(vreinterpretq_u32_s32(_reg[0]), first._reg[0], sec._reg[0]);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    // Blend is same for signed and unsigned
                    return _mm_blendv_epi8(sec._reg, first._reg, _reg);
    #elif defined SYMD_NEON
                    return vbsl_u8(_reg, first._reg, sec._reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return typename UnderlyingRegister<T>::Type{
    #ifdef SYMD_SSE
                       _mm256_blendv_pd(sec._reg[0], first._reg[0], _reg[0]),
                       _mm256_blendv_pd(sec._reg[1], first._reg[1], _reg[1])
    #elif defined SYMD_NEON
                        vbslq_f64(vreinterpretq_u64_f64(_reg[0]), first._reg[0], sec._reg[0]),
                        vbslq_f64(vreinterpretq_u64_f64(_reg[1]), first._reg[1], sec._reg[1])
    #endif
                    };
                }
            }

            SymdRegister abs() const
            {
                if constexpr (std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    return this->max(-*this);
    #elif defined SYMD_NEON
                    return vabsq_f32(_reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    return _mm256_abs_epi32(_reg);
    #elif defined SYMD_NEON
                    return vabsq_s32(_reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
                    // If x is usnigned than x == abs(x)
                    return *this;
                }
                else if constexpr (std::is_same_v<T, double>)
                {
    #ifdef SYMD_SSE
                    return this->max(-*this);
    #elif defined SYMD_NEON
                    return typename UnderlyingRegister<T>::Type {
                        vabsq_f64(_reg[0]),
                        vabsq_f64(_reg[1]),
                    };
    #endif
                }
            }

            void store(T* dst) const
            {
                if constexpr (std::is_same_v<T, float>)
                {
    #ifdef SYMD_SSE
                    _mm256_storeu_ps(dst, _reg);
    #elif defined SYMD_NEON
                    vst1q_f32(dst, _reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, symd::bfloat16>)
                {
    #ifdef SYMD_SSE
                    __m256i int_reg = _mm256_castps_si256(_reg);
                    __m256i shuffled_hi = _mm256_shufflehi_epi16(int_reg, 0b00001101);
                    __m256i shuffled_16 = _mm256_shufflelo_epi16(shuffled_hi, 0b00001101);
                    __m256i shuffled_32 = _mm256_shuffle_epi32(shuffled_16, 0b11111000);
                    __m256i shuffled_64 = _mm256_permute4x64_epi64(shuffled_32, 0b11111000);

                    __m128i final_data = _mm256_extracti128_si256(shuffled_64, 0);
                    _mm_store_si128((__m128i*)dst, final_data);

    #elif defined SYMD_NEON
                    assert(false);
    #endif
                }
                else if constexpr (std::is_same_v<T, int>)
                {
    #ifdef SYMD_SSE
                    _mm256_storeu_si256((__m256i*)dst, _reg);
    #elif defined SYMD_NEON
                    vst1q_s32(dst, _reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char>)
                {
    #ifdef SYMD_SSE
                    _mm_storeu_si64((__m128i*)dst, _reg);
    #elif defined SYMD_NEON
                    std::memcpy(dst, _ptrToData, SYMD_LEN * sizeof(T));
    #endif
                }
                else if constexpr (std::is_same_v<T, double>)
                {
    #ifdef SYMD_SSE
                    _mm256_storeu_pd(dst + 0, _reg[0]);
                    _mm256_storeu_pd(dst + 4, _reg[1]);
    #elif defined SYMD_NEON
                    vst1q_f64(dst + 0, _reg[0]);
                    vst1q_f64(dst + 2, _reg[1]);
    #endif
                }
            }

            typename UnderlyingRegister<T>::DType operator[](size_t ind) const
            {
                return _ptrToData[ind];
            }


            /////////////////////////////////////////////////////////////////////////////////////
            // Conversions
            /////////////////////////////////////////////////////////////////////////////////////

            template <typename R>
            SymdRegister<R> convert_to() const
            {
                if constexpr (std::is_same_v<T, float> && std::is_same_v<R, symd::bfloat16>)
                {
                    // Float -> bfloat16 ------------------------------------------------------------
                    return _reg;
                }
                else if constexpr (std::is_same_v<T, symd::bfloat16> && std::is_same_v<R, float>)
                {
                    // bfloat16 -> Float ------------------------------------------------------------
                    return _reg;
                }
                else if constexpr ((std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>) 
                    && std::is_same_v<R, int>)
                {
                    // Float,bfloat16 -> int ------------------------------------------------------------
    #ifdef SYMD_SSE
                    return _mm256_cvtps_epi32(_reg);
    #elif defined SYMD_NEON
                    return vcvtq_s32_f32(_reg);
    #endif
                }
                else if constexpr ((std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>) 
                    && std::is_same_v<R, unsigned char>)
                {
                    // Float,bfloat16 -> unsigned char ------------------------------------------------------------
                    return this->convert_to<int>().template convert_to<unsigned char>();
                }
                else if constexpr ((std::is_same_v<T, float> || std::is_same_v<T, symd::bfloat16>) 
                    && std::is_same_v<R, double>)
                {
                    // Float,bfloat16 -> Double ------------------------------------------------------------
    #ifdef SYMD_SSE
                    __m128 hi = _mm256_extractf128_ps(_reg, 1);
                    __m128 lo = _mm256_castps256_ps128(_reg);

                    return typename UnderlyingRegister<double>::Type{ _mm256_cvtps_pd(lo), _mm256_cvtps_pd(hi) };
    #elif defined SYMD_NEON

                    auto lo = vcvt_f64_f32(vget_low_f32(_reg));
                    auto hi = vcvt_f64_f32(vget_high_f32(_reg));

                    return typename UnderlyingRegister<double>::Type{ lo, hi };
    #endif
                }
                else if constexpr (std::is_same_v<T, double> && 
                    (std::is_same_v<R, float> || std::is_same_v<R, symd::bfloat16>))
                {
                    // Double -> Float,bfloat16 ------------------------------------------------------------
    #ifdef SYMD_SSE
                    return _mm256_set_m128(_mm256_cvtpd_ps(_reg[1]), _mm256_cvtpd_ps(_reg[0]));
    #elif defined SYMD_NEON

                    return vcombine_f32(vcvt_f32_f64(_reg[0]), vcvt_f32_f64(_reg[1]));
    #endif
                }
                else if constexpr (std::is_same_v<T, double> && std::is_same_v<R, int>)
                {
                    // Double -> int ------------------------------------------------------------
    #ifdef SYMD_SSE
                    __m128i lo = _mm256_cvtpd_epi32(_reg[0]);
                    __m128i hi = _mm256_cvtpd_epi32(_reg[1]);

                    return _mm256_set_m128i(hi, lo);
    #elif defined SYMD_NEON

                    auto lo = vcvt_s32_f32(vcvt_f32_f64(_reg[0]));
                    auto hi = vcvt_s32_f32(vcvt_f32_f64(_reg[1]));

                    return vcombine_s32(lo, hi);
    #endif
                }
                else if constexpr (std::is_same_v<T, double> && std::is_same_v<R, unsigned char>)
                {
                    // double -> unsigned char ------------------------------------------------------------
                    return this->convert_to<int>().template convert_to<unsigned char>();
                }
                else if constexpr (std::is_same_v<T, int> && 
                    (std::is_same_v<R, float> || std::is_same_v<R, symd::bfloat16>))
                {
                    // Int -> float,bfloat16 ------------------------------------------------------------
    #ifdef SYMD_SSE
                    return _mm256_cvtepi32_ps(_reg);
    #elif defined SYMD_NEON
                    return  avcvtq_f32_s32(_reg);
    #endif
                }
                else if constexpr (std::is_same_v<T, int> && std::is_same_v<R, unsigned char>)
                {
                    // Int -> unsigned char ------------------------------------------------------------
    #ifdef SYMD_SSE
                    __m128i lo = _mm256_extractf128_si256(_reg, 0);
                    __m128i hi = _mm256_extractf128_si256(_reg, 1);

                    __m128i _8shorts = _mm_packs_epi32(lo, hi);
                    return _mm_packus_epi16(_8shorts, _8shorts);
    #elif defined SYMD_NEON
                    uint16x4_t hi = vqmovun_s32(_reg[0]);
                    uint16x4_t lo = vqmovun_s32(_reg[1]);
                    return vqmovn_u16(vcombine_u16(hi, lo));
    #endif
                }
                else if constexpr (std::is_same_v<T, int> && std::is_same_v<R, double>)
                {
                    // Int -> double ------------------------------------------------------------
                    return typename UnderlyingRegister<double>::Type {
    #ifdef SYMD_SSE
                        _mm256_cvtepi32_pd(_mm256_extractf128_si256(_reg, 0)),
                        _mm256_cvtepi32_pd(_mm256_extractf128_si256(_reg, 1))
    #elif defined SYMD_NEON
                        vcvtq_f64_s64(vmovl_s32(vget_low_s32(_reg))),
                        vcvtq_f64_s64(vmovl_s32(vget_high_s32(_reg)))
    #endif
                    };
                }
                else if constexpr (std::is_same_v<T, unsigned char> && 
                    (std::is_same_v<R, float> || std::is_same_v<R, symd::bfloat16>))
                {
                    // unsigned char -> float,bfloat16 ------------------------------------------------------------
                    return this->convert_to<int>().template convert_to<float>()._reg;
                }
                else if constexpr (std::is_same_v<T, unsigned char> && std::is_same_v<R, int>)
                {
                    // unsigned char -> int ------------------------------------------------------------
    #ifdef SYMD_SSE
                    __m128i zeros = _mm_setzero_si128();
                    __m128i shorts = _mm_cvtepu8_epi16(_reg);

                    __m128i lo = _mm_unpacklo_epi16(shorts, zeros);
                    __m128i hi = _mm_unpackhi_epi16(shorts, zeros);

                    return _mm256_set_m128i(hi, lo);
    #elif defined SYMD_NEON
                    uint16x8_t shorts = vmovl_u8(_reg);
                    uint32x4_t ints = vmovl_u16(vget_low_u16(shorts));
                    return vreinterpretq_s32_u32(ints);
    #endif
                }
                else if constexpr (std::is_same_v<T, unsigned char> && std::is_same_v<R, double>)
                {
                    // unsigned char -> double ------------------------------------------------------------
                    return this->convert_to<int>().template convert_to<double>();
                }
                else
                {
                    return *this;
                }
            };
        };


        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Arrithmetic opperators
        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T>
        SymdRegister<T>& operator+(const SymdRegister<T>& x)
        {
            return x;
        }

        template <typename T>
        SymdRegister<T>& operator-(const SymdRegister<T>& x)
        {
            return SymdRegister<T>((T)0) - x;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Assignment opperators
        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T>
        SymdRegister<T>& operator+=(SymdRegister<T>& first, const SymdRegister<T>& sec)
        {
            first = first + sec;
            return first;
        }

        template <typename T>
        SymdRegister<T>& operator-=(SymdRegister<T>& first, const SymdRegister<T>& sec)
        {
            first = first - sec;
            return first;
        }

        template <typename T>
        SymdRegister<T>& operator*=(SymdRegister<T>& first, const SymdRegister<T>& sec)
        {
            first = first * sec;
            return first;
        }

        template <typename T>
        SymdRegister<T>& operator/=(SymdRegister<T>& first, const SymdRegister<T>& sec)
        {
            first = first / sec;
            return first;
        }

        template <typename T>
        SymdRegister<T>& operator&=(SymdRegister<T>& first, const SymdRegister<T>& sec)
        {
            first = first & sec;
            return first;
        }

        template <typename T>
        SymdRegister<T>& operator|=(SymdRegister<T>& first, const SymdRegister<T>& sec)
        {
            first = first | sec;
            return first;
        }

        template <typename T>
        SymdRegister<T>& operator^=(SymdRegister<T>& first, const SymdRegister<T>& sec)
        {
            first = first ^ sec;
            return first;
        }

        template <typename T>
        SymdRegister<T>& operator<<=(SymdRegister<T>& first, int num_bits)
        {
            first = first << num_bits;
            return first;
        }

        template <typename T>
        SymdRegister<T>& operator>>=(SymdRegister<T>& first, int num_bits)
        {
            first = first >> num_bits;
            return first;
        }


        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Operators with scalars
        ////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T, typename U>
        SymdRegister<T> operator+(U first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>((T)first) + sec;
        }

        template <typename T, typename U>
        SymdRegister<T> operator-(U first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>((U)first) - sec;
        }

        template <typename T, typename U>
        SymdRegister<T> operator*(U first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>((U)first) * sec;
        }

        template <typename T, typename U>
        SymdRegister<T> operator/(U first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>((U)first) / sec;
        }

        template <typename T, typename U>
        SymdRegister<T> operator&(U first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>((U)first) & sec;
        }

        template <typename T, typename U>
        SymdRegister<T> operator|(U first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>((U)first) | sec;
        }

        template <typename T, typename U>
        SymdRegister<T> operator^(U first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>((U)first) ^ sec;
        }

        // Comparison

        template <typename T>
        SymdRegister<T> operator==(T first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>(first) == sec;
        }

        template <typename T>
        SymdRegister<T> operator==(const SymdRegister<T>& x, T y)
        {
            return x == SymdRegister<T>(y);
        }

        template <typename T>
        SymdRegister<T> operator!=(T first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>(first) != sec;
        }

        template <typename T>
        SymdRegister<T> operator!=(const SymdRegister<T>& x, T y)
        {
            return x != SymdRegister<T>(y);
        }

        template <typename T>
        SymdRegister<T> operator<(T first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>(first) != sec;
        }

        template <typename T>
        SymdRegister<T> operator>(T first, const SymdRegister<T>& sec)
        {
            return SymdRegister<T>(first) != sec;
        }
    } // __internal__
} // symd


///////////////////////////////////////////////////////////////////////////////////////////////////
// STD functions (min max abs)
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace std
{
    using namespace symd::__internal__;

    /// <summary>
    /// Returns SymdRegister containing minimum values from two input SymdRegisters.
    /// </summary>
    template <typename T>
    inline SymdRegister<T> min(SymdRegister<T> first, SymdRegister<T> sec)
    {
        return first.min(sec);
    }

    /// <summary>
    /// Returns SymdRegister containing minimum values from input SymdRegister and constant.
    /// </summary>
    template <typename T>
    inline SymdRegister<T> min(SymdRegister<T> first, T sec)
    {
        return first.min(SymdRegister<T>(sec));
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // max
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /// <summary>
    /// Returns SymdRegister containing maximum values from two input SymdRegisters.
    /// </summary>
    template <typename T>
    inline SymdRegister<T> max(SymdRegister<T> first, SymdRegister<T> sec)
    {
        return first.max(sec);
    }

    /// <summary>
    /// Returns SymdRegister containing maximum values from input SymdRegister and constant.
    /// </summary>
    template <typename T>
    inline SymdRegister<T> max(SymdRegister<T> first, T sec)
    {
        return first.max(SymdRegister<T>(sec));
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // abs
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /// <summary>
    /// Returns SymdRegister containing abs values from input SymdRegister.
    /// </summary>
    template <typename T>
    SymdRegister<T> abs(SymdRegister<T> reg)
    {
        return reg.abs();
    }
}
