/////////////////////////////////////////////////////////////////////////////////////////
// Utilities
// V.0.3.8 2022-07-22
// MIT License
// Copyright (c) 2022 unmanned
/////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <math.h>
#include <stdint.h>
#include "fr32/fr32.h"

#ifndef SAMPLE_RATE 
#define SAMPLE_RATE 44100
#endif
#define PI  3.141592653589793238462f
#define PI2 PI/2.0f
#define TAO 6.283185307179586476925f

/////////////////////////////////////////////////////////////////////////////////////////
// https://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf ////////////////////////////
typedef struct 
{
    float ic1eq;
    float ic2eq;
    float g;
    float k;
    float a;
    float b;
    float low;
    float band;
    float high;
    float ftable[128];

} ltosvf;

void svflto_clr(ltosvf* o);

void svflto_init(ltosvf* o, int cutoff, float Q);

void svflto_process(ltosvf* o, float in);

/////////////////////////////////////////////////////////////////////////////////////////
// One pole LP parameter smooth filter //////////////////////////////////////////////////
typedef struct  
{
    int a;
    int b;
    int o;

} psf;

void psf_init(psf* o, int time);

int psf_process(psf* o, int in);


/////////////////////////////////////////////////////////////////////////////////////////
// Envelope follower ////////////////////////////////////////////////////////////////////
typedef struct
{
    float a;
    float r;
    float envelope;

} ef;

void ef_init(ef* o, float aMs, float rMs);

void ef_process(ef* o, float in);

/////////////////////////////////////////////////////////////////////////////////////////
// Limiter //////////////////////////////////////////////////////////////////////////////
typedef struct 
{
    ef e;
    float threshold;

} limiter;


void limiter_init(limiter* o, float aMs, float rMs, float threshold);

float limit(limiter* o, float in);

/////////////////////////////////////////////////////////////////////////////////////////
// Gator ////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    int eax;

} gator;


int gate(gator* o, int in, int width);

int rand_in_range(int l, int r);


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    int t;
    int value;

} snh;

void snh_init(snh* o);

int snh_process(snh* o, int input, int time);

/////////////////////////////////////////////////////////////////////////////////////////