#pragma once
#include "oscillator.h"
#include "sequencer.h"
#include "utility.h"
#include "fr32/fr32.h"

#define oscn 3

typedef struct
{
    oscillator osc[oscn];
    sequencer sq;
    limiter lim;
    ltosvf svf;
    int    cvs[8]; // CV Sources
    float  cva[8]; // CV Amounts
    int    pot[9]; // CV Centres
    int    vcfid;  // Filter type
    float* q;
    float* f;
    float  feed;

} spawner;

void spawner_init(spawner* o);
void modulate(spawner* o);
void spawn(spawner* o);


float fHP(spawner*, float);
float fBP(spawner*, float);
float fNotch(spawner*, float);
float fPeak(spawner*, float);
float fLP(spawner*, float);
