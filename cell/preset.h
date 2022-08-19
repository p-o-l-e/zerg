#pragma once
#include <stdbool.h>
#include <string.h>
#define oscn 3

typedef struct
{
    uint16_t eax[17];// Pot memory
    int      cvs[8]; // CV Sources
    float    cva[8]; // CV Amounts
    int      pot[9]; // CV Centres
    int      vcfid;  // Filter type
    float    freq[oscn];
    uint16_t seq[12];
    float    note[STEPS];
    float    amp[oscn];
    int      form[oscn];
    int      oct[oscn];           // Octave
    bool     sqon;
    bool     envon;

} preset;