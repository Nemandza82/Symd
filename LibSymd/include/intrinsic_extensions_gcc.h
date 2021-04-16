#pragma once
#ifndef _WIN32

__attribute__((always_inline)) inline
__m128i _mm_loadu_si64(void const* mem_addr)
{
    __m128i ret;
    asm("movq %1, %0" : "=x" (ret) : "m" (*(int64_t*)mem_addr));
    return ret;
}

__attribute__((always_inline)) inline
__m128i _mm_loadu_si128(__m128i const* mem_addr)
{
    __m128i ret;
    asm("movdqu %1, %0" : "=x" (ret) : "m" (*mem_addr));
    return ret;
}

__attribute__((always_inline)) inline
__m256 _mm256_set_m128(__m128 hi, __m128 lo)
{
    __m256 ret;
    asm("vinsertf128 $1, %x1, %t2, %t0" : "=x" (ret) : "x" (hi), "x" (lo));
    return ret;
}

__attribute__((always_inline)) inline
__m256i _mm256_set_m128i(__m128i hi, __m128i lo)
{
    __m256i ret;
    asm("vinsertf128 $1, %x1, %t2, %t0" : "=x" (ret) : "x" (hi), "x" (lo));
    return ret;
}

#endif