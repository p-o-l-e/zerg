#include "envelope.h"

void ar_setf(ar* o)
{
    o->fa = 1.0f / (float)o->a;
    o->fr = 1.0f / (float)o->r;
}


void ar_init(ar* o)
{
    o->stage = 0;
    o->depated = 0;
    o->feed = 0.0f;
    ar_setf(o);
}


float ar_process(ar* o)
{
    if(o->stage == 0) // Attack
    {
        o->feed += o->fa;
        if(o->depated == o->a) 
        {
            o->stage++;
            o->depated = 0;
        }
    }
    else if(o->stage == 1) // Release
    {
        o->feed -= o->fr;
        if(o->depated == o->r)
        {
            o->stage++;
            o->feed = 0.0f;
        }
    }
    o->depated++;
    return o->feed;
}