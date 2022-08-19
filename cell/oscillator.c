////////////////////////////////////////////////////////////////////////////////////////
// Oscillator
// V.0.3.8 2022-08-10
////////////////////////////////////////////////////////////////////////////////////////
#include "oscillator.h"
#include "scales.h"

// const float octave[] = { 0.0001f, 0.8593, 6.875f, 27.5f, 55.0f, 110.0f, 220.0f, 440.0f, 880.0f, 1760.0f, 3520.0f, 7040.0f };
// const float octave[] = { 0.0001f, 27.5f, 440.0f, 3520.0f};
const float octave[] = { 0.0f, 24.0f, 96.0f, 192.0f };
float get_chroma(int n) { return chromatic[n]; };
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void oscillator_init(oscillator* o)
{ 
    o->phase = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void set_delta(oscillator* o)
{ 
    int f  = (int)(octave[*o->oct] + (octave[*o->oct + 1] - octave[*o->oct])/4096.0f * (float)*o->f);
    o->delta = chromatic[f + o->shift] * TAO / SAMPLE_RATE;
}

////////////////////////////////////////////////////////////////////////////////////////
// Waveforms: VCO //////////////////////////////////////////////////////////////////////
void oRamp(oscillator* o) 
{
    o->out  =  o->phase/PI;
    o->out *= *o->amp + fabsf(*o->am**o->aa);

    o->phase += o->delta + fabsf(*o->fm**o->fa);
    while(o->phase > PI) o->phase -= TAO;
}

void oSaw(oscillator* o) 
{
    o->out  = -o->phase/PI;
    o->out *= *o->amp + fabsf(*o->am**o->aa);

    o->phase += o->delta + fabsf(*o->fm**o->fa);
    while(o->phase > PI) o->phase -= TAO;
}

void oSquare(oscillator* o)
{
    o->out  =  o->phase > 0.0f ? -1.0f : 1.0f;
    o->out *= *o->amp + fabsf(*o->am**o->aa);

    o->phase += o->delta + fabsf(*o->fm**o->fa);
    while(o->phase > PI) o->phase -= TAO;
}

void oTriangle(oscillator* o)
{
    o->out  = 2.0f * (fabsf(o->phase / PI) - 0.5f);
    o->out *= *o->amp + fabsf(*o->am**o->aa);

    o->phase += o->delta + fabsf(*o->fm**o->fa);
    while(o->phase > PI) o->phase -= TAO;
}

void oNoise(oscillator* o)
{
    o->out  = (float)rand()/(float)(RAND_MAX) * 2.0f - 1.0f;
    o->out *= *o->amp + fabsf(*o->am**o->aa);
}

void oRandStep(oscillator* o)
{
    static float eax;
    o->out = eax;
    o->out   *= *o->amp   + fabsf(*o->am**o->aa);
    o->phase +=  o->delta + fabsf(*o->fm**o->fa);
    while(o->phase > PI) 
    {
        o->phase -= TAO;
        eax = (float)rand()/(float)(RAND_MAX) * 2.0f - 1.0f;
    }
}