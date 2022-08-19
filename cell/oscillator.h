////////////////////////////////////////////////////////////////////////////////////////
// Oscillator
// V.0.3.8 2022-08-10
////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <math.h>
#include "fr32/fr32.h"
#include <stdio.h>                                                              
#include <stdlib.h>  


#ifndef SAMPLE_RATE 
#define SAMPLE_RATE 24000
#endif

#define TAO 6.283185307179586476925f
#define PI  3.141592653589793238462f
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

typedef struct
{
    float* f;             // Frequency
    float  phase;         // Current phase
    float  delta;         // Phase increment
    float* amp;           // Overall amplitude
    float* fm;            // Frequency modulation
    float* fa;            // FM Amount
    float* am;            // Amplitude modulation
    float* aa;            // AM Amount
    float* pwm;           // Pulse width modulation: 0 < 1 
    float* pwa;           // PWM Amount
    float  out;           // Output: x, y, z
    int*   form;
    int*   oct;           // Octave
    int    shift;         // Frequency shift

} oscillator; 

void oscillator_init(oscillator*);
void set_delta(oscillator*);
float get_chroma(int n);

void oRamp     (oscillator*);
void oSaw      (oscillator*);
void oSquare   (oscillator*);
void oTriangle (oscillator*);
void oNoise    (oscillator*);
void oRandStep (oscillator*);
