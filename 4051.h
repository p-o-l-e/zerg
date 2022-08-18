#pragma once
#include "pico/stdlib.h"

#define PIN_A 20
#define PIN_B 21
#define PIN_C 22

void set4051_0(unsigned st)
{
    gpio_put(PIN_A, 0);
    gpio_put(PIN_B, 0);
    gpio_put(PIN_C, 0);
    sleep_us(st);
}

void set4051_1(unsigned st)
{
    gpio_put(PIN_A, 0);
    gpio_put(PIN_B, 0);
    gpio_put(PIN_C, 1);
    sleep_us(st);
}

void set4051_2(unsigned st)
{
    gpio_put(PIN_A, 0);
    gpio_put(PIN_B, 1);
    gpio_put(PIN_C, 0);
    sleep_us(st);
}

void set4051_3(unsigned st)
{
    gpio_put(PIN_A, 0);
    gpio_put(PIN_B, 1);
    gpio_put(PIN_C, 1);
    sleep_us(st);
}

void set4051_4(unsigned st)
{
    gpio_put(PIN_A, 1);
    gpio_put(PIN_B, 0);
    gpio_put(PIN_C, 0);
    sleep_us(st);
}

void set4051_5(unsigned st)
{
    gpio_put(PIN_A, 1);
    gpio_put(PIN_B, 0);
    gpio_put(PIN_C, 1);
    sleep_us(st);
}

void set4051_6(unsigned st)
{
    gpio_put(PIN_A, 1);
    gpio_put(PIN_B, 1);
    gpio_put(PIN_C, 0);
    sleep_us(st);
}

void set4051_7(unsigned st)
{
    gpio_put(PIN_A, 1);
    gpio_put(PIN_B, 1);
    gpio_put(PIN_C, 1);
    sleep_us(st);
}