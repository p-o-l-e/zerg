#include "fr32.h"

int32_t float_to_fr32(float real, uint8_t sft) 
{
    return ((int32_t)((1U << sft) * (real)));
}

int32_t float_to_fr32_pih(uint8_t sft) 
{
    return float_to_fr32((float)M_PIH, sft);
}

int32_t double_to_fr32(double real, uint8_t sft) 
{
    return ((int32_t)((1U << sft) * (real)));
}

int32_t double_to_fr32_pih(uint8_t sft)
{
    return double_to_fr32(M_PIH, sft);
}

float fr32_to_float(int32_t fr32, uint8_t sft)
{
    return ((float)fr32 * 1.0F / (1U << sft));
}

double fr32_to_double(int32_t fr32, uint8_t sft) 
{
    return ((double)fr32 * 1.0F / (1U << sft));
}

int32_t mul_fr32(int32_t a, int32_t b, uint8_t sft) 
{
    return (int32_t)(((int64_t)a * (int64_t)b) >> sft);
}

int32_t div_fr32(int32_t a, int32_t b, uint8_t sft) 
{
    return (int32_t)(((int64_t)a << sft) / b);
}

static int32_t cos_fr32_q30(int32_t x) 
{
    static const uint8_t sft = 30;
    static const int32_t a[6] = 
    {
        0x20000000,
        0x05555558,
        0x02222224,
        0x0124924A,
        0x00B60B61,
        0x007C1F08,
    };
    uint8_t i;
    int32_t xxh, tmp, y;

    xxh = mul_fr32(x, x >> 1U, sft);
    tmp = double_to_fr32(1.0F, sft);
    y = tmp;

    for (i = 1; i <= sizeof(a) / sizeof(a[0]); i++) 
    {
        tmp = mul_fr32(mul_fr32(-tmp, xxh, sft), a[i-1], sft) << 1U;
        y += tmp;
    }
    return y;
}

int32_t cos_fr32(int32_t x, uint8_t sft) 
{
    uint8_t lsft;
    int32_t lpih;
    int8_t sign = 1;

    if (29 <= sft) return 0;

    lpih = double_to_fr32_pih(sft);
    lsft = 30 - sft;

    if (x & 0x80000000) x = -x;

    while (((lpih << 1U) + lpih) < x) x -= lpih << 2;

    if (lpih <= x)
    {
        sign *= -1;
        x -= lpih << 1U;
    }

    x = x << lsft;

    return sign * cos_fr32_q30(x) >> lsft;
}

int32_t sin_fr32(int32_t x, uint8_t sft) 
{
    return cos_fr32(x - float_to_fr32_pih(sft), sft); 
}