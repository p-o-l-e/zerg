#pragma once
#include <stdbool.h>

typedef struct 
{
    int   a; // Timings
    int   r;
    int   stage;
    int   depated;

    float fa;
    float fr;
    float feed;
    bool* on;

} ar;

void ar_init(ar* o);
void ar_setf(ar* o);
float ar_process(ar* o);