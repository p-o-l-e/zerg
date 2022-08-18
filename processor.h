#pragma once
#include "feeder.h"
#include "4051.h"
#include "littlefs/fio.h"
#define LAG4051  100


static spawner o;
#define NPSF 9
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


int loadInit()
{
    // mount the filesystem
    int err = lfs_mount(&lfs, &CFG);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) 
    {
        lfs_format(&lfs, &CFG);
        lfs_mount(&lfs, &CFG);
        // create the boot count file

        lfs_file_open (&lfs, &INIT, "PRESETS", LFS_O_RDWR | LFS_O_CREAT);
        lfs_file_write(&lfs, &INIT, &o.pset, sizeof(o.pset));
        lfs_file_close(&lfs, &INIT);
    }
    // read current count
    lfs_file_open(&lfs, &INIT, "PRESETS", LFS_O_RDWR);
    lfs_file_read(&lfs, &INIT, &o.pset, sizeof(o.pset));

    // update boot count
    // lfs_file_rewind(&lfs, &INIT);
    // lfs_file_write(&lfs, &INIT, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &INIT);
    // release any resources we were using
    lfs_unmount(&lfs);

}

int saveInit()
{
    int err = lfs_mount(&lfs, &CFG);
    lfs_file_open (&lfs, &INIT, "PRESETS", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_write(&lfs, &INIT, &o.pset, sizeof(o.pset));
    lfs_file_close(&lfs, &INIT);
    lfs_unmount(&lfs);
}