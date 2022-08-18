#pragma once

#include <stdint.h>

#define FMSB  31
#define SFT   24
#define PI24  0x3243f6c
#define TAO24 0x6487ed8
#define ONE   4096
#define ONE24 16777216


#define SFT_LIMIT(sft) (((uint8_t)(sft) > (FMSB)) ? (FMSB) : (uint8_t)(sft))

#define M_PIH 1.5707963267341256

int32_t float_to_fr32(float, uint8_t);
int32_t float_to_fr32_pih(uint8_t);
int32_t double_to_fr32(double, uint8_t);
int32_t double_to_fr32_pih(uint8_t);
float   fr32_to_float(int32_t, uint8_t);
double  fr32_to_double(int32_t, uint8_t);

int32_t mul_fr32(int32_t, int32_t, uint8_t);
int32_t div_fr32(int32_t, int32_t, uint8_t);

int32_t cos_fr32(int32_t, uint8_t);
int32_t sin_fr32(int32_t, uint8_t);
