#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "pico/audio_i2s.h"
#include "pico-ss-oled/include/ss_oled.h"
#include "hardware/adc.h"
#include "pico/multicore.h"
#include "processor.h"


#define SAMPLES_PER_BUFFER 512
#define DIN_PIN     2
#define BCLK_PIN    0
#define BUTTON_C    17
#define BUTTON_B    18
#define BUTTON_A    19
#define SDA_PIN     4
#define SCL_PIN     5
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define RESET_PIN   -1

#define PAGES   4
#define ITEMS   6
#define FILES   24



SSOLED oled;
const char*   msrc[] = {"SCA", "SCB", "SCC", "SEQ", "ENV"};
const char*     ft[] = {"LPF", "BPF", "NCH", "PCK", "HPF"};
const char*    oct[] = {" 0 ", " 1 ", " 2 " };
const char*     wf[] = {"SQR", "RMP", "SAW", "TRI", "NSE", "SNH" };
const char*   name[] = { "[01]", "[02]", "[03]", "[04]", 
                         "[05]", "[06]", "[07]", "[08]",
                         "[09]", "[0A]", "[0B]", "[0C]", 
                         "[0D]", "[0E]", "[0F]", "[10]",
                         
                         "[11]", "[12]", "[13]", "[14]",
                         "[15]", "[16]", "[17]", "[18]",
                         "[19]", "[1A]", "[1B]", "[1C]",
                         "[1D]", "[1E]", "[1F]", "[20]" };
#define ENTRIES 18
static uint16_t eap[ENTRIES];
static bool sqpart;
static int init_file;


int16_t entry_handler(int16_t pot_value, int16_t in_value, int_fast8_t pos, int16_t min, int16_t max)
{
    in_value += (pot_value - eap[pos]);
    if(in_value > max) in_value = max;
    if(in_value < min) in_value = min;
    eap[pos] = pot_value;
    
    return in_value;
}

#define BARS 12
static char buf[BARS];
static char vbuf[7];
static char runline_a[6];
static char runline_b[6];



char* progress(int value)
{
    // ox84
    float v = (float)value * (float)BARS  / 4096.0f; // n Bars
    int   q = (int)v;      // quotient
    int   r = (v - q) * 5; // remainder

    r = roundf((float)r /(float)BARS * 5.0f);

    int i = 0;
    for(; i < q; i++)
    {
        buf[i] = '\x88';
    }

    int c = 0x84 + r;
    buf[i] = c;
    i++;

    for(; i < BARS; i++)
    {
        buf[i] = ' ';
    }
    return buf;
}



char* vprogress(int value)
{
    // ox84
    float v = (float)value * (float)7.0f  / 4096.0f; // n Bars
    int   q = (int)v;      // quotient
    int   r = (v - q) * 5; // remainder

    r = roundf((float)r /8.0f * 5.0f);

    int i = 6;
    for(; i > q; i--)
    {
        vbuf[i] = '\x8D';
    }

    int c = 0x8D - r;
    vbuf[i] = c;
    i--;

    for(; i >= 0; i--)
    {
        vbuf[i] = ' ';
    }
    return vbuf;
}

void sqrun(int pos)
{
    for(int i = 0; i < 6; i++)
    {
        runline_a[i] = ' ';
        runline_b[i] = ' ';
    }
    if(pos>5)  runline_b[pos-6] = '\x91';
    else       runline_a[pos  ] = '\x91';
    

}
////////////////////////////////////////////////////////////////////////////////////
// Core 1 interrupt Handler ////////////////////////////////////////////////////////
void core1_interrupt_handler() 
{
    static int_fast8_t state_a = 0;
    static int_fast8_t state_b = 0;
    static int_fast8_t state_c = 0;
    static int_fast8_t page    = 0;
    static int_fast8_t item;
    static int_fast8_t entry;

    while (multicore_fifo_rvalid())
    {
        uint16_t raw = multicore_fifo_pop_blocking();   

        if(gpio_get(BUTTON_A)) 
        {
            if(state_a == 0) state_a = 1;
        } 
        else 
        {
            state_a = 0;
        }

        if(gpio_get(BUTTON_B)) 
        {
            if(state_b == 0) state_b = 1;
        } 
        else 
        {
            state_b = 0;
        }

        if(gpio_get(BUTTON_C)) 
        {
            if(state_c == 0) state_c = 1;
        } 
        else 
        {
            state_c = 0;
        }

        if(state_a == 1)
        {
            page++;
            if(page >= PAGES) page = 0;
            state_a = 2;
        }

        if(state_b == 1)
        {
            if(page > 3)
            {
                entry++;
                if(entry == FILES) entry = 0;
            }
            else
            {
                item++;
                if(item == ITEMS) item = 0;
            }
            state_b = 2;
        }

        if(state_c == 1)
        {
            if(page == 0)
            {
                o.pset.oct[item%3]++;
                if( o.pset.oct[item%3] == 3)  o.pset.oct[item%3] = 0;
            }

            else if(page == 1)
            {
                if(item<3)
                {
                    o.pset.form[item]++;
                    if( o.pset.form[item] == 6) o.pset.form[item] = 0;
                }
                else // AM : CVS 0-1-2
                {
                    o.pset.cvs[item - 3]++;
                    if(o.pset.cvs[item - 3] == 5)  o.pset.cvs[item - 3] = 0;
                }
            }

            else if(page == 2)
            {
                if(item==0)
                {
                    o.pset.vcfid++;
                    if(o.pset.vcfid == 5) o.pset.vcfid = 0;
                }
                else if(item==1) // Q
                {
                    o.pset.cvs[3]++;
                    if(o.pset.cvs[3]==5) o.pset.cvs[3] = 0;
                }
                else if(item==2) // Cutoff
                {
                    o.pset.cvs[4]++;
                    if(o.pset.cvs[4]==5) o.pset.cvs[4] = 0;
                }
                else // FM CVS : 5-6-7
                {
                    o.pset.cvs[item+2]++;
                    if( o.pset.cvs[item+2] == 5)  o.pset.cvs[item+2] = 0;
                }
            }
            else if(page == 3)
            {
                if     (item%3 == 0) *o.sq.on = !*o.sq.on;
                else if(item%3 == 1) *o.sq.env.on = !*o.sq.env.on;
                else                  sqpart = !sqpart;
            }
            else if(page == 4)
            {
                sleep_ms(300);
                saveFile(entry, name[entry]);
                sleep_ms(300);
                page = 0;
            }
            else if(page == 5)
            {
                sleep_ms(300);
                loadFile(entry, name[entry]);
                sleep_ms(300);
                page = 0;
            }
            state_c = 2;
        }


        if((state_a == 2)&&(state_c == 2))
        {
            if(page == 4) { sleep_ms(600); page = 0; }
            else          { sleep_ms(600); page = 4; }
        }

        if((state_a == 2)&&(state_b == 2))
        {
            if(page == 5) { sleep_ms(600); page = 0; }
            else          { sleep_ms(600); page = 5; }
        }

        // RANDOMIZE //////////////////////////////////////////////////////////////////////
        if((state_b == 2)&&(state_c == 2))
        {
            sleep_ms(400);
            o.sq.length = rand_in_range(2, ONE24);
            genRand(&o.sq);
            for(int i = 0; i < 12; i++)
            {
                o.pset.seq[i] = o.sq.note[i] * 4096.0f;
            }

            for(int i = 0; i < oscn; i++)
            {
                o.pset.freq[i] = rand_in_range(1, 4096);
                o.pset.oct[i]  = rand_in_range(0, 2);
                o.pset.form[i] = rand_in_range(0, 4);

                o.pset.eax[i + 3]  = (float)rand_in_range(1, 4096); // Volume
                o.pset.amp[i]      = (float)o.pset.eax[i + 3]/4096.0f;

                o.pset.eax[i + 11] = (float)rand_in_range(1, 4096); // FM
                o.pset.cva[i + 5]  = o.pset.eax[i + 11]/4096.0f;    // FM

                o.pset.eax[i + 6]  = (float)rand_in_range(1, 4096); // AM
                o.pset.cva[i]      = o.pset.eax[i + 6]/4096.0f;     // AM

                o.pset.cvs[i]      = rand_in_range(0, 4); // AM
                o.pset.cvs[i + 5]  = rand_in_range(0, 4); // FM

                set_delta(&o);

                if     (o.pset.cvs[i    ] == 3) o.osc[i].am = &o.sq.out;
                else if(o.pset.cvs[i    ] == 4) o.osc[i].am = &o.sq.env.feed;
                else                            o.osc[i].am = &o.osc[o.pset.cvs[i    ]].out;

                if     (o.pset.cvs[i + 5] == 3) o.osc[i].fm = &o.sq.out;
                else if(o.pset.cvs[i + 5] == 4) o.osc[i].fm = &o.sq.env.feed;
                else                            o.osc[i].fm = &o.osc[o.pset.cvs[i + 5]].out;
            }

            o.pset.eax[ 9] = (float)rand_in_range(1, 4096); // Q
            o.pset.cvs[3] = rand_in_range(0, 4);

            if     (o.pset.cvs[3] == 3) o.q = &o.sq.out;
            else if(o.pset.cvs[3] == 4) o.q = &o.sq.env.feed;
            else                        o.q = &o.osc[o.pset.cvs[3]].out;

            o.pset.eax[10] = (float)rand_in_range(1, 4096); // Cutoff
            o.pset.cvs[4] = rand_in_range(0, 4);

            o.pset.cva[3] = (float)o.pset.eax[ 9]/4096.0f;
            o.pset.cva[4] = (float)o.pset.eax[10]/4096.0f; // Cutoff

            if     (o.pset.cvs[4] == 3) o.f = &o.sq.out;
            else if(o.pset.cvs[4] == 4) o.f = &o.sq.env.feed;
            else                        o.f = &o.osc[o.pset.cvs[4]].out;

            o.pset.eax[0] = rand_in_range(1, 4096);
            o.pset.eax[1] = rand_in_range(1, 4096);
            o.pset.eax[2] = rand_in_range(1, 4096);

            o.sq.env.a  = o.pset.eax[0]*32;
            o.sq.env.r  = o.pset.eax[1]*32;
            o.sq.length = o.pset.eax[2]/32 + 1;

            o.pset.vcfid  = rand_in_range(0, 4);

            *o.sq.on     = rand()&1;
            *o.sq.env.on = rand()&1;
            sqpart       = rand()&1;

            ar_setf(&o.sq.env);

            page = 0;
            item = 0;
        }


        if(page == 0)
        {
            o.pset.freq[0] = entry_handler(o.pset.pot[0], o.pset.freq[0], 0, 1, 4096); // fA
            o.pset.freq[1] = entry_handler(o.pset.pot[1], o.pset.freq[1], 1, 1, 4096); // fB
            o.pset.freq[2] = entry_handler(o.pset.pot[2], o.pset.freq[2], 2, 1, 4096); // fC

            o.pset.eax[0]  = entry_handler(o.pset.pot[3], o.pset.eax[0], 3, 1, 4096); // Attack
            o.pset.eax[1]  = entry_handler(o.pset.pot[4], o.pset.eax[1], 4, 1, 4096); // Release
            o.pset.eax[2]  = entry_handler(o.pset.pot[5], o.pset.eax[2], 5, 1, 4096); // BPM

            oledWriteString(&oled, 0,  0, 0,  "   |TUNE        ", 1, 0, 1);

            oledWriteString(&oled, 0,  0, 1, oct[o.pset.oct[0]], 1, item%3==0, 1);
            oledWriteString(&oled, 0, 32, 1, progress(o.pset.freq[0]), 1, 0, 1);
            
            oledWriteString(&oled, 0,  0, 2, oct[o.pset.oct[1]], 1, item%3==1, 1);
            oledWriteString(&oled, 0, 32, 2, progress(o.pset.freq[1]), 1, 0, 1);

            oledWriteString(&oled, 0,  0, 3, oct[o.pset.oct[2]], 1, item%3==2, 1);
            oledWriteString(&oled, 0, 32, 3, progress(o.pset.freq[2]), 1, 0, 1);
            
            oledWriteString(&oled, 0,  0, 4, "   |ENVELOPE    ", 1, 0, 1);

            oledWriteString(&oled, 0,  0, 5, "ATK", 1, 0, 1);
            oledWriteString(&oled, 0, 32, 5, progress(o.pset.eax[0]), 1, 0, 1);

            oledWriteString(&oled, 0,  0, 6, "RLS", 1, 0, 1);
            oledWriteString(&oled, 0, 32, 6, progress(o.pset.eax[1]), 1, 0, 1);

            oledWriteString(&oled, 0,  0, 7, "BPM", 1, 0, 1);
            oledWriteString(&oled, 0, 32, 7, progress(o.pset.eax[2]), 1, 0, 1);

            o.sq.env.a  = o.pset.eax[0]*32;
            o.sq.env.r  = o.pset.eax[1]*32;
            o.sq.length = o.pset.eax[2]/32 + 1;
            ar_setf(&o.sq.env);
            for(int i = 1; i < 8; i++)
            {
                oledWriteString(&oled, 0, 24, i,  "|", 1, 0, 1);
            }
        }


        else if(page == 1)
        {
            // Amplitude ///////////////////////////////////////////////////////////
            o.pset.eax[3] = entry_handler(o.pset.pot[0], o.pset.eax[3], 0, 1, 4096);
            o.pset.eax[4] = entry_handler(o.pset.pot[1], o.pset.eax[4], 1, 1, 4096);
            o.pset.eax[5] = entry_handler(o.pset.pot[2], o.pset.eax[5], 2, 1, 4096);
            // AM //////////////////////////////////////////////////////////////////
            o.pset.eax[6] = entry_handler(o.pset.pot[3], o.pset.eax[6], 3, 1, 4096);
            o.pset.eax[7] = entry_handler(o.pset.pot[4], o.pset.eax[7], 4, 1, 4096);
            o.pset.eax[8] = entry_handler(o.pset.pot[5], o.pset.eax[8], 5, 1, 4096);


            oledWriteString(&oled, 0,  0, 0,  "   |AMP         ", 1, 0, 1);

            oledWriteString(&oled, 0,  0, 1, wf[o.pset.form[0]], 1, item==0, 1);
            oledWriteString(&oled, 0, 32, 1, progress(o.pset.eax[3]), 1, 0, 1);

            oledWriteString(&oled, 0,  0, 2, wf[o.pset.form[1]], 1, item==1, 1);
            oledWriteString(&oled, 0, 32, 2, progress(o.pset.eax[4]), 1, 0, 1);

            oledWriteString(&oled, 0,  0, 3, wf[o.pset.form[2]], 1, item==2, 1);
            oledWriteString(&oled, 0, 32, 3, progress(o.pset.eax[5]), 1, 0, 1);


            oledWriteString(&oled, 0,  0, 4,  "   |AM          ", 1, 0, 1);

            oledWriteString(&oled, 0,  0, 5, msrc[o.pset.cvs[0]], 1, item==3, 1);
            oledWriteString(&oled, 0, 32, 5, progress(o.pset.eax[6]), 1, 0, 1);

            oledWriteString(&oled, 0,  0, 6, msrc[o.pset.cvs[1]], 1, item==4, 1);
            oledWriteString(&oled, 0, 32, 6, progress(o.pset.eax[7]), 1, 0, 1);

            oledWriteString(&oled, 0,  0, 7, msrc[o.pset.cvs[2]], 1, item==5, 1);
            oledWriteString(&oled, 0, 32, 7, progress(o.pset.eax[8]), 1, 0, 1);

            for(int i = 1; i < 8; i++)
            {
                oledWriteString(&oled, 0, 24, i,  "|", 1, 0, 1);
            }
            o.pset.amp[0] = (float)o.pset.eax[3]/4096.0f;
            o.pset.amp[1] = (float)o.pset.eax[4]/4096.0f;
            o.pset.amp[2] = (float)o.pset.eax[5]/4096.0f;

            o.pset.cva[0] = (float)o.pset.eax[6]/4096.0f;
            o.pset.cva[1] = (float)o.pset.eax[7]/4096.0f;
            o.pset.cva[2] = (float)o.pset.eax[8]/4096.0f;

            for(int i = 0; i < 3; i++)
            {
                if     (o.pset.cvs[i] == 3) o.osc[i].am = &o.sq.out;
                else if(o.pset.cvs[i] == 4) o.osc[i].am = &o.sq.env.feed;
                else                        o.osc[i].am = &o.osc[o.pset.cvs[i]].out;
            }
        }

        else if(page == 2)
        {
            // FILTER //////////////////////////////////////////////////////////////////////////
            o.pset.eax[ 9] = entry_handler(o.pset.pot[1], o.pset.eax[ 9], 1, 1, 4096); // Q
            o.pset.eax[10] = entry_handler(o.pset.pot[2], o.pset.eax[10], 2, 1, 4096); // Cutoff
            // FM //////////////////////////////////////////////////////////////////////////////
            o.pset.eax[11] = entry_handler(o.pset.pot[3], o.pset.eax[11], 3, 1, 4096);
            o.pset.eax[12] = entry_handler(o.pset.pot[4], o.pset.eax[12], 4, 1, 4096);
            o.pset.eax[13] = entry_handler(o.pset.pot[5], o.pset.eax[13], 5, 1, 4096);


            oledWriteString(&oled, 0,  0, 0,  "   |VCF         ", 1, 0, 1);

            oledWriteString(&oled, 0,  0, 1, ft[o.pset.vcfid], 1, item==0, 1);
            oledWriteString(&oled, 0, 32, 1, "             ", 1, 0, 1);

            oledWriteString(&oled, 0,  0, 2, msrc[o.pset.cvs[3]], 1, item == 1, 1);
            oledWriteString(&oled, 0, 32, 2, progress(o.pset.eax[9]), 1, 0, 1);

            oledWriteString(&oled, 0,  0, 3, msrc[o.pset.cvs[4]], 1, item == 2, 1);
            oledWriteString(&oled, 0, 32, 3, progress(o.pset.eax[10]), 1, 0, 1);


            oledWriteString(&oled, 0,  0, 4,  "   |FM          ", 1, 0, 1);

            oledWriteString(&oled, 0,  0, 5, msrc[o.pset.cvs[5]], 1, item == 3, 1);
            oledWriteString(&oled, 0, 32, 5, progress(o.pset.eax[11]), 1, 0, 1);

            oledWriteString(&oled, 0,  0, 6, msrc[o.pset.cvs[6]], 1, item == 4, 1);
            oledWriteString(&oled, 0, 32, 6, progress(o.pset.eax[12]), 1, 0, 1);

            oledWriteString(&oled, 0,  0, 7, msrc[o.pset.cvs[7]], 1, item == 5, 1);
            oledWriteString(&oled, 0, 32, 7, progress(o.pset.eax[13]), 1, 0, 1);

            for(int i = 1; i < 8; i++)
            {
                oledWriteString(&oled, 0, 24, i,  "|", 1, 0, 1);
            }

            o.pset.cva[3] = (float)o.pset.eax[ 9]/4096.0f;
            o.pset.cva[4] = (float)o.pset.eax[10]/4096.0f;

            o.pset.cva[5] = (float)o.pset.eax[11]/4096.0f;
            o.pset.cva[6] = (float)o.pset.eax[12]/4096.0f;
            o.pset.cva[7] = (float)o.pset.eax[13]/4096.0f;

            if     (o.pset.cvs[3] == 3) o.q = &o.sq.out;
            else if(o.pset.cvs[3] == 4) o.q = &o.sq.env.feed;
            else                        o.q = &o.osc[o.pset.cvs[3]].out;

            if     (o.pset.cvs[4] == 3) o.f = &o.sq.out;
            else if(o.pset.cvs[4] == 4) o.f = &o.sq.env.feed;
            else                        o.f = &o.osc[o.pset.cvs[4]].out;

            for(int i = 0; i < 3; i++)
            {
                if     (o.pset.cvs[i + 5] == 3) o.osc[i].fm = &o.sq.out;
                else if(o.pset.cvs[i + 5] == 4) o.osc[i].fm = &o.sq.env.feed;
                else                            o.osc[i].fm = &o.osc[o.pset.cvs[i + 5]].out;
            }
        }

        else if(page==3)
        {
            int seg = (int)sqpart;

            for(int i = 0; i < 6; i++)
            {
                o.pset.seq[i + 6 * seg] = entry_handler(o.pset.pot[i], o.pset.seq[i  + 6 * seg], i, 1, 4096);
            }

            for(int i = 0; i < 12; i++)
            {
                oledWriteStringV(&oled, 0, i * 8 + 32, 0,  vprogress(4096 - o.pset.seq[i]), 1, 0, 1);
                o.sq.note[i] = (float)o.pset.seq[i]/4096.0f;
            }
            sqrun(o.sq.current);
            oledWriteString(&oled, 0, 32, 7,  runline_a, 1, !sqpart, 1);
            oledWriteString(&oled, 0, 80, 7,  runline_b, 1,  sqpart, 1);

            oledWriteString(&oled, 0, 0, 0,  "SEQ", 1, item%3 == 0, 1);
            *o.sq.on ? oledWriteString(&oled, 0, 0, 1,  " ON", 1, 0, 1) : oledWriteString(&oled, 0, 0, 1,  "OFF", 1, 0, 1);
            oledWriteString(&oled, 0, 0, 2,  "   ", 1, 0, 1);

            oledWriteString(&oled, 0, 0, 3,  "ENV", 1, item%3 == 1, 1);
            *o.sq.env.on? oledWriteString(&oled, 0, 0, 4,  " ON", 1, 0, 1) : oledWriteString(&oled, 0, 0, 4,  "OFF", 1, 0, 1);

            sqpart ? oledWriteString(&oled, 0, 0, 7,  "  B", 1, item%3 == 2, 1) : oledWriteString(&oled, 0, 0, 7,  "A  ", 1, item%3 == 2, 1);

            for(int i = 5; i < 7; i++)
            {
                oledWriteString(&oled, 0, 0, i,  "   ", 1, 0, 1);
            }
        }

        else if(page == -1)
        {
               
            oledWriteString(&oled, 0, 0, 0, "FFFF    V. 1.0.A", 1, 0, 1);
            oledWriteString(&oled, 0, 0, 1, "2022    UNMANNED", 1, 0, 1);

            // oledWriteString(&oled, 0, 0, 2, "\x94\x94\x94\x94\x94\x94\x94\x94\x94\x94\x94\x94\x94\x94\x94\x94", 1, 0, 1);
            oledWriteString(&oled, 0, 0, 3, "\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92", 1, 0, 1);
            oledWriteString(&oled, 0, 0, 4, "\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92", 1, 0, 1);
            oledWriteString(&oled, 0, 0, 5, "\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92", 1, 0, 1);
            oledWriteString(&oled, 0, 0, 6, "\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92", 1, 0, 1);
            oledWriteString(&oled, 0, 0, 7, "\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92\x92", 1, 0, 1);

        }

        else if(page == 4)
        {
            oledWriteString(&oled, 0, 0, 0, "SAVE :          ", 1, 0, 1);
            oledWriteString(&oled, 0, 0, 1, "                ", 1, 0, 1);

            for(int c = 0; c < 4; c++)
            {
                for(int i = 0; i < 6; i++)
                {
                    oledWriteString(&oled, 0,  32 * c, i + 2, name[i + c * 6], 1, entry == (i + c * 6), 1);
                }
            }
        }


        else if(page == 5)
        {
            oledWriteString(&oled, 0, 0, 0, "LOAD :          ", 1, 0, 1);
            oledWriteString(&oled, 0, 0, 1, "                ", 1, 0, 1);

            for(int c = 0; c < 4; c++)
            {
                for(int i = 0; i < 6; i++)
                {
                    oledWriteString(&oled, 0,  32 * c, i + 2, name[i + c * 6], 1, entry == (i + c * 6), 1);
                }
            }
        }

        get_parameters();
        modulate(&o);
    }
    multicore_fifo_clear_irq(); // Clear interrupt
}

////////////////////////////////////////////////////////////////////////////////////
// Core 1 Main Code ////////////////////////////////////////////////////////////////
void core1_entry() 
{
    // Configure Core 1 Interrupt
    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC1, core1_interrupt_handler);
    irq_set_enabled(SIO_IRQ_PROC1, true);
    // Infinte While Loop to wait for interrupt
    while (true)
    {
        tight_loop_contents();
    }
}


int main()
{
	stdio_init_all();
	multicore_launch_core1(core1_entry);
    bool clk = set_sys_clock_khz(150000, true);

    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_init(BUTTON_C);

    gpio_init(20);
    gpio_init(21);
    gpio_init(22);

    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_set_dir(BUTTON_C, GPIO_IN);

    gpio_set_dir(20, GPIO_OUT);
    gpio_set_dir(21, GPIO_OUT);
    gpio_set_dir(22, GPIO_OUT);
        
    int rc = oledInit(&oled, OLED_128x64, 0x3c, 0, 0, 1, SDA_PIN, SCL_PIN, RESET_PIN, 1000000L);

    oledFill(&oled, 0,1);
    oledSetContrast(&oled, 127);

    adc_init();
    adc_set_temp_sensor_enabled(true);

    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);

	for(int i = 0; i < NPSF; i++)
    {
        psf_init(&PSF[i], ONE24/16*15);
    }

	spawner_init(&o);

    struct audio_buffer_pool *ap = init_audio_i2s(SAMPLES_PER_BUFFER, DIN_PIN, BCLK_PIN);

    adc_select_input(4);
    srand(adc_read());
    oledFill(&oled, 0,1);
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    loadInit();
  
    sleep_ms(300);
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////

    while (true) 
	{
        give_audio_buffer(ap, fill_next_buffer(&o, ap, SAMPLES_PER_BUFFER));
        uint16_t raw = 1;
        multicore_fifo_push_blocking(raw);
    }
    return 0;
}