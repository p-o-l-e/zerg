#define oscn 3

typedef struct
{
    int    cvs[8]; // CV Sources
    float  cva[8]; // CV Amounts
    int    pot[9]; // CV Centres
    int    vcfid;  // Filter type
    float  freq[oscn];

} preset;