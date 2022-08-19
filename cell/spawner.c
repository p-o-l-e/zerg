#include "spawner.h"

float fHP(spawner* s, float in)
{
    svflto_process(&s->svf, in);
    return s->svf.high;
}

float fBP(spawner* s, float in)
{
    svflto_process(&s->svf, in);
    return s->svf.band;
}

float fNotch(spawner* s, float in)
{
    svflto_process(&s->svf, in);
    return s->svf.low + s->svf.high;
}

float fPeak(spawner* s, float in)
{
    svflto_process(&s->svf, in);
    return s->svf.low - s->svf.high;
}

float fLP(spawner* s, float in)
{
    svflto_process(&s->svf, in);
    return s->svf.low;
}

void (*form[])(oscillator*) = 
{    
    oSquare,    // 0
    oRamp,      // 1
    oSaw,       // 2
    oTriangle,  // 3
    oNoise,     // 4
    oRandStep   // 5
};

float (*filter[])(spawner*, float) = 
{
    fLP,
    fBP,
    fNotch,
    fPeak,
    fHP
};





void spawner_init(spawner* o)
{
    for(int i = 0; i < oscn; i++)
    {
        o->osc[i].pwm = &o->osc[i].out;
        o->osc[i].f   = &o->pset.freq[i];
        o->osc[i].pwa = &o->pset.cva[6 + i];
        o->osc[i].fm  = &o->osc[i].out;
        o->osc[i].am  = &o->osc[i].out;
        o->osc[i].aa  = &o->pset.cva[3 + i];
        o->osc[i].fa  = &o->pset.cva[i];
        o->osc[i].form= &o->pset.form[i];
        o->osc[i].amp = &o->pset.amp[i];
        o->osc[i].oct = &o->pset.oct[i];
        oscillator_init(&o->osc[i]);
    }
    o->sq.on = &o->pset.sqon;
    o->sq.env.on = &o->pset.envon;
    o->sq.note = o->pset.note;
    o->f = &o->osc[0].out;
    o->q = &o->osc[0].out;
    o->pset.vcfid = 1;


    for(int i = 0; i < 128; i++)
    {
        o->svf.ftable[i] = tanf(PI * get_chroma(i + 48) / SAMPLE_RATE);
    }
    init_sequence(&o->sq, 32);
    limiter_init(&o->lim, 0.1f, 1.0f, 1.0f);
}


void modulate(spawner* o)
{
    process_sequence(&o->sq);
    for(int i = 0; i < oscn; i++)
    {
        if(*o->sq.on) o->osc[i].shift = roundf(get_note(&o->sq) * 24.0f);
        else          o->osc[i].shift = 0;

        set_delta(&o->osc[i]);
    }
}

void spawn(spawner* o)
{
    o->feed = 0.0f;
    ar_process(&o->sq.env);
    for(int i = 0; i < oscn; i++)
    {
        form[o->pset.form[i]](&o->osc[i]);
        o->feed += o->osc[i].out;
    }
    if(*o->sq.env.on) o->feed *= o->sq.env.feed;
    o->feed  = limit(&o->lim, o->feed);

    int c = o->pset.pot[6] / 32 + (int)(*o->f*o->pset.cva[4]*128.0f);
    if (c < 0) c = 0; else if (c > 127) c = 127;

    float q = (float)o->pset.pot[7]/1365.0f + *o->q*o->pset.cva[3]*3.2f;
    if (q < 0.2f) q = 0.2f; else if (q > 3.2)  q = 3.2f;

    svflto_init(&o->svf, c, q);
    o->feed  = filter[o->pset.vcfid](o, o->feed);
}