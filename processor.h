#pragma once
#include "feeder.h"
#include "4051.h"
#include "littlefs/fio.h"
#define LAG4051   100


static spawner o;
#define NPSF      9
#define THRESHOLD 8
static psf   PSF[NPSF]; // Parameter smooth filters
static gator GTR[NPSF];

static lfs_t      lfs;
static lfs_file_t INIT;


void get_parameters()
{
    // Upper row //////////////////////////////////////////////////////////////////////////////
    // Cutoff /////////////////////////////////////////////////////////////////////////////////
    adc_select_input(1);
    o.pset.pot[6] = gate(&GTR[6], psf_process(&PSF[6], adc_read()), THRESHOLD); // Cutoff
    // 4051 ///////////////////////////////////////////////////////////////////////////////////
    adc_select_input(2);
    // Volume /////////////////////////////////////////////////////////////////////////////////
    set4051_1(LAG4051);
    o.pset.pot[8] = gate(&GTR[1], adc_read(), THRESHOLD);
    // Resonance //////////////////////////////////////////////////////////////////////////////
    set4051_3(LAG4051);
    o.pset.pot[7] = gate(&GTR[0], psf_process(&PSF[0], adc_read()), THRESHOLD); // Q

    // Lower row //////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    set4051_7(LAG4051);
    o.pset.pot[0] = gate(&GTR[7], psf_process(&PSF[7], adc_read()), THRESHOLD);
    ///////////////////////////////////////////////////////////////////////////////////////////
    set4051_5(LAG4051);
    o.pset.pot[1] = gate(&GTR[5], psf_process(&PSF[5], adc_read()), THRESHOLD);
    ///////////////////////////////////////////////////////////////////////////////////////////
    set4051_2(LAG4051);
    o.pset.pot[2] = gate(&GTR[2], psf_process(&PSF[2], adc_read()), THRESHOLD);
    ///////////////////////////////////////////////////////////////////////////////////////////
    set4051_4(LAG4051);
    o.pset.pot[3] = gate(&GTR[4], psf_process(&PSF[4], adc_read()), THRESHOLD);
    ///////////////////////////////////////////////////////////////////////////////////////////
    set4051_0(LAG4051);
    o.pset.pot[4] = gate(&GTR[8], psf_process(&PSF[8], adc_read()), THRESHOLD);
    ///////////////////////////////////////////////////////////////////////////////////////////
    set4051_6(LAG4051);
    o.pset.pot[5] = gate(&GTR[3], psf_process(&PSF[3], adc_read()), THRESHOLD); 

}


void set_parameters()
{
    for(int i = 0; i < oscn; i++)
    {
        set_delta(&o);

        if     (o.pset.cvs[i    ] == 3) o.osc[i].am = &o.sq.out;
        else if(o.pset.cvs[i    ] == 4) o.osc[i].am = &o.sq.env.feed;
        else                            o.osc[i].am = &o.osc[o.pset.cvs[i    ]].out;

        if     (o.pset.cvs[i + 5] == 3) o.osc[i].fm = &o.sq.out;
        else if(o.pset.cvs[i + 5] == 4) o.osc[i].fm = &o.sq.env.feed;
        else                            o.osc[i].fm = &o.osc[o.pset.cvs[i + 5]].out;
    }

    if     (o.pset.cvs[3] == 3) o.q = &o.sq.out;
    else if(o.pset.cvs[3] == 4) o.q = &o.sq.env.feed;
    else                        o.q = &o.osc[o.pset.cvs[3]].out;

    if     (o.pset.cvs[4] == 3) o.f = &o.sq.out;
    else if(o.pset.cvs[4] == 4) o.f = &o.sq.env.feed;
    else                        o.f = &o.osc[o.pset.cvs[4]].out;

    o.sq.env.a  = o.pset.eax[0]*32;
    o.sq.env.r  = o.pset.eax[1]*32;
    o.sq.length = o.pset.eax[2]/32 + 1;

    ar_setf(&o.sq.env);

}

int loadInit()
{
    int err = lfs_mount(&lfs, &CFG);

    if (err) 
    {
        lfs_format(&lfs, &CFG);
        lfs_mount (&lfs, &CFG);

        lfs_file_open (&lfs, &INIT, "PRESETS", LFS_O_RDWR | LFS_O_CREAT);
        lfs_file_write(&lfs, &INIT, &o.pset, sizeof(o.pset));
        lfs_file_close(&lfs, &INIT);
    }
    lfs_file_open(&lfs, &INIT, "PRESETS", LFS_O_RDWR);
    lfs_file_read(&lfs, &INIT, &o.pset, sizeof(o.pset));

    lfs_file_close(&lfs, &INIT);
    lfs_unmount(&lfs);
    set_parameters();
}



int loadFile(int n, const char* path)
{

    int err = lfs_mount(&lfs, &CFG);
    if (err) 
    {
        lfs_format(&lfs, &CFG);
        lfs_mount (&lfs, &CFG);

        lfs_file_open (&lfs, &INIT, path, LFS_O_RDWR | LFS_O_CREAT);
        lfs_file_write(&lfs, &INIT, &o.pset, sizeof(o.pset));
        lfs_file_close(&lfs, &INIT);
    }

    if(lfs_file_open (&lfs, &INIT, path, LFS_O_RDWR) == 0)
    {
        lfs_file_read (&lfs, &INIT, &o.pset, sizeof(o.pset));
        lfs_file_close(&lfs, &INIT);
    }

    lfs_unmount(&lfs);
    set_parameters();
}


int saveFile(int n, const char* path)
{
    int err = lfs_mount(&lfs, &CFG);
    lfs_file_open (&lfs, &INIT, path, LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_write(&lfs, &INIT, &o.pset, sizeof(o.pset));
    lfs_file_close(&lfs, &INIT);
    lfs_unmount(&lfs);
    return err;
}


int saveInit()
{
    int err = lfs_mount(&lfs, &CFG);
    lfs_file_open (&lfs, &INIT, "PRESETS", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_write(&lfs, &INIT, &o.pset, sizeof(o.pset));
    lfs_file_close(&lfs, &INIT);
    lfs_unmount(&lfs);
    return err;
}
