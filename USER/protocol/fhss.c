#include "fhss.h"

int32_t FreqCorrection = 0;
uint8_t volatile FHSSptr = 0;

// Our table of FHSS frequencies. Define a regulatory domain to select the correct set for your location and radio
#ifdef Regulatory_Domain_EU_868
/* Frequency bands taken from https://wetten.overheid.nl/BWBR0036378/2016-12-28#Bijlagen
 * Note: these frequencies fall in the license free H-band, but in combination with 500kHz
 * LoRa modem bandwidth used by ExpressLRS (EU allows up to 125kHz modulation BW only) they
 * will never pass RED certification and they are ILLEGAL to use.
 *
 * Therefore we simply maximize the usage of available spectrum so laboratory testing of the software won't disturb existing
 * 868MHz ISM band traffic too much.
 */
const uint32_t FHSSfreqs[] = {
    FREQ_HZ_TO_REG_VAL_900(863275000), // band H1, 863 - 865MHz, 0.1% duty cycle or CSMA techniques, 25mW EIRP
    FREQ_HZ_TO_REG_VAL_900(863800000),
    FREQ_HZ_TO_REG_VAL_900(864325000),
    FREQ_HZ_TO_REG_VAL_900(864850000),
    FREQ_HZ_TO_REG_VAL_900(865375000), // Band H2, 865 - 868.6MHz, 1.0% dutycycle or CSMA, 25mW EIRP
    FREQ_HZ_TO_REG_VAL_900(865900000),
    FREQ_HZ_TO_REG_VAL_900(866425000),
    FREQ_HZ_TO_REG_VAL_900(866950000),
    FREQ_HZ_TO_REG_VAL_900(867475000),
    FREQ_HZ_TO_REG_VAL_900(868000000),
    FREQ_HZ_TO_REG_VAL_900(868525000), // Band H3, 868.7-869.2MHz, 0.1% dutycycle or CSMA, 25mW EIRP
    FREQ_HZ_TO_REG_VAL_900(869050000),
    FREQ_HZ_TO_REG_VAL_900(869575000)};
#elif defined Regulatory_Domain_FCC_915
/* Very definitely not fully checked. An initial pass at increasing the hops
*/
const uint32_t FHSSfreqs[] = {
    FREQ_HZ_TO_REG_VAL_900(903500000),
    FREQ_HZ_TO_REG_VAL_900(904100000),
    FREQ_HZ_TO_REG_VAL_900(904700000),
    FREQ_HZ_TO_REG_VAL_900(905300000),

    FREQ_HZ_TO_REG_VAL_900(905900000),
    FREQ_HZ_TO_REG_VAL_900(906500000),
    FREQ_HZ_TO_REG_VAL_900(907100000),
    FREQ_HZ_TO_REG_VAL_900(907700000),

    FREQ_HZ_TO_REG_VAL_900(908300000),
    FREQ_HZ_TO_REG_VAL_900(908900000),
    FREQ_HZ_TO_REG_VAL_900(909500000),
    FREQ_HZ_TO_REG_VAL_900(910100000),

    FREQ_HZ_TO_REG_VAL_900(910700000),
    FREQ_HZ_TO_REG_VAL_900(911300000),
    FREQ_HZ_TO_REG_VAL_900(911900000),
    FREQ_HZ_TO_REG_VAL_900(912500000),

    FREQ_HZ_TO_REG_VAL_900(913100000),
    FREQ_HZ_TO_REG_VAL_900(913700000),
    FREQ_HZ_TO_REG_VAL_900(914300000),
    FREQ_HZ_TO_REG_VAL_900(914900000),

    FREQ_HZ_TO_REG_VAL_900(915500000), // as per AU..
    FREQ_HZ_TO_REG_VAL_900(916100000),
    FREQ_HZ_TO_REG_VAL_900(916700000),
    FREQ_HZ_TO_REG_VAL_900(917300000),

    FREQ_HZ_TO_REG_VAL_900(917900000),
    FREQ_HZ_TO_REG_VAL_900(918500000),
    FREQ_HZ_TO_REG_VAL_900(919100000),
    FREQ_HZ_TO_REG_VAL_900(919700000),

    FREQ_HZ_TO_REG_VAL_900(920300000),
    FREQ_HZ_TO_REG_VAL_900(920900000),
    FREQ_HZ_TO_REG_VAL_900(921500000),
    FREQ_HZ_TO_REG_VAL_900(922100000),

    FREQ_HZ_TO_REG_VAL_900(922700000),
    FREQ_HZ_TO_REG_VAL_900(923300000),
    FREQ_HZ_TO_REG_VAL_900(923900000),
    FREQ_HZ_TO_REG_VAL_900(924500000),

    FREQ_HZ_TO_REG_VAL_900(925100000),
    FREQ_HZ_TO_REG_VAL_900(925700000),
    FREQ_HZ_TO_REG_VAL_900(926300000),
    FREQ_HZ_TO_REG_VAL_900(926900000)};
#elif Regulatory_Domain_ISM_2400
uint32_t FHSSfreqs[] = {
    FREQ_HZ_TO_REG_VAL_24(2400400000),
    FREQ_HZ_TO_REG_VAL_24(2401400000),
    FREQ_HZ_TO_REG_VAL_24(2402400000),
    FREQ_HZ_TO_REG_VAL_24(2403400000),
    FREQ_HZ_TO_REG_VAL_24(2404400000),

    FREQ_HZ_TO_REG_VAL_24(2405400000),
    FREQ_HZ_TO_REG_VAL_24(2406400000),
    FREQ_HZ_TO_REG_VAL_24(2407400000),
    FREQ_HZ_TO_REG_VAL_24(2408400000),
    FREQ_HZ_TO_REG_VAL_24(2409400000),

    FREQ_HZ_TO_REG_VAL_24(2410400000),
    FREQ_HZ_TO_REG_VAL_24(2411400000),
    FREQ_HZ_TO_REG_VAL_24(2412400000),
    FREQ_HZ_TO_REG_VAL_24(2413400000),
    FREQ_HZ_TO_REG_VAL_24(2414400000),

    FREQ_HZ_TO_REG_VAL_24(2415400000),
    FREQ_HZ_TO_REG_VAL_24(2416400000),
    FREQ_HZ_TO_REG_VAL_24(2417400000),
    FREQ_HZ_TO_REG_VAL_24(2418400000),
    FREQ_HZ_TO_REG_VAL_24(2419400000),

    FREQ_HZ_TO_REG_VAL_24(2420400000),
    FREQ_HZ_TO_REG_VAL_24(2421400000),
    FREQ_HZ_TO_REG_VAL_24(2422400000),
    FREQ_HZ_TO_REG_VAL_24(2423400000),
    FREQ_HZ_TO_REG_VAL_24(2424400000),

    FREQ_HZ_TO_REG_VAL_24(2425400000),
    FREQ_HZ_TO_REG_VAL_24(2426400000),
    FREQ_HZ_TO_REG_VAL_24(2427400000),
    FREQ_HZ_TO_REG_VAL_24(2428400000),
    FREQ_HZ_TO_REG_VAL_24(2429400000),

    FREQ_HZ_TO_REG_VAL_24(2430400000),
    FREQ_HZ_TO_REG_VAL_24(2431400000),
    FREQ_HZ_TO_REG_VAL_24(2432400000),
    FREQ_HZ_TO_REG_VAL_24(2433400000),
    FREQ_HZ_TO_REG_VAL_24(2434400000),

    FREQ_HZ_TO_REG_VAL_24(2435400000),
    FREQ_HZ_TO_REG_VAL_24(2436400000),
    FREQ_HZ_TO_REG_VAL_24(2437400000),
    FREQ_HZ_TO_REG_VAL_24(2438400000),
    FREQ_HZ_TO_REG_VAL_24(2439400000),

    FREQ_HZ_TO_REG_VAL_24(2440400000),
    FREQ_HZ_TO_REG_VAL_24(2441400000),
    FREQ_HZ_TO_REG_VAL_24(2442400000),
    FREQ_HZ_TO_REG_VAL_24(2443400000),
    FREQ_HZ_TO_REG_VAL_24(2444400000),

    FREQ_HZ_TO_REG_VAL_24(2445400000),
    FREQ_HZ_TO_REG_VAL_24(2446400000),
    FREQ_HZ_TO_REG_VAL_24(2447400000),
    FREQ_HZ_TO_REG_VAL_24(2448400000),
    FREQ_HZ_TO_REG_VAL_24(2449400000),

    FREQ_HZ_TO_REG_VAL_24(2450400000),
    FREQ_HZ_TO_REG_VAL_24(2451400000),
    FREQ_HZ_TO_REG_VAL_24(2452400000),
    FREQ_HZ_TO_REG_VAL_24(2453400000),
    FREQ_HZ_TO_REG_VAL_24(2454400000),

    FREQ_HZ_TO_REG_VAL_24(2455400000),
    FREQ_HZ_TO_REG_VAL_24(2456400000),
    FREQ_HZ_TO_REG_VAL_24(2457400000),
    FREQ_HZ_TO_REG_VAL_24(2458400000),
    FREQ_HZ_TO_REG_VAL_24(2459400000),

    FREQ_HZ_TO_REG_VAL_24(2460400000),
    FREQ_HZ_TO_REG_VAL_24(2461400000),
    FREQ_HZ_TO_REG_VAL_24(2462400000),
    FREQ_HZ_TO_REG_VAL_24(2463400000),
    FREQ_HZ_TO_REG_VAL_24(2464400000),

    FREQ_HZ_TO_REG_VAL_24(2465400000),
    FREQ_HZ_TO_REG_VAL_24(2466400000),
    FREQ_HZ_TO_REG_VAL_24(2467400000),
    FREQ_HZ_TO_REG_VAL_24(2468400000),
    FREQ_HZ_TO_REG_VAL_24(2469400000),

    FREQ_HZ_TO_REG_VAL_24(2470400000),
    FREQ_HZ_TO_REG_VAL_24(2471400000),
    FREQ_HZ_TO_REG_VAL_24(2472400000),
    FREQ_HZ_TO_REG_VAL_24(2473400000),
    FREQ_HZ_TO_REG_VAL_24(2474400000),

    FREQ_HZ_TO_REG_VAL_24(2475400000),
    FREQ_HZ_TO_REG_VAL_24(2476400000),
    FREQ_HZ_TO_REG_VAL_24(2477400000),
    FREQ_HZ_TO_REG_VAL_24(2478400000),
    FREQ_HZ_TO_REG_VAL_24(2479400000)};
#else
#error No regulatory domain defined, please define one in user_defines.txt
#endif

uint8_t sync_channel;
#define FHSS_FREQ_CNT  80
#define  FHSS_SEQUENCE_CNT  240
uint8_t FHSSsequence[FHSS_SEQUENCE_CNT] = {0};  
extern uint8_t FHSSsequence[FHSS_SEQUENCE_CNT];    
    
void FHSSsetCurrIndex(uint8_t value)
{ 
    // set the current index of the FHSS pointer
    FHSSptr = value;
}

uint8_t FHSSgetCurrIndex()
{ 
    // get the current index of the FHSS pointer
    return FHSSptr;
}

uint32_t GetInitialFreq()
{
    return FHSSfreqs[sync_channel] - FreqCorrection;  
}

uint32_t FHSSgetNextFreq()
{
    FHSSptr = (FHSSptr + 1) % FHSS_SEQUENCE_CNT;
    uint32_t freq = FHSSfreqs[FHSSsequence[FHSSptr]] - FreqCorrection;
    return freq;
}


static unsigned long seed = 0;


uint16_t rng(void)
{
    const uint32_t m = 2147483648;
    const uint32_t a = 214013;
    const uint32_t c = 2531011;
    seed = (a * seed + c) % m;
    return seed >> 16;
}


static unsigned int rngN(unsigned int max)
{
    return rng() % max;
}

// Set all of the flags in the array to true, except for the first one
// which corresponds to the sync channel and is never available for normal
// allocation.
void resetIsAvailable(uint8_t *array)
{
    // channel 0 is the sync channel and is never considered available
    array[0] = 0;

    // all other entires to 1
    for (unsigned int i = 1; i < NR_FHSS_ENTRIES; i++)
        array[i] = 1;
}

void rngSeed(const uint32_t newSeed)
{
    seed = newSeed;
}

void FHSSrandomiseFHSSsequence(const uint32_t seed)
{


//    DBGLN("Number of FHSS frequencies = %u", FHSS_FREQ_CNT);

    sync_channel = FHSS_FREQ_CNT / 2;
//    DBGLN("Sync channel = %u", sync_channel);

    // reset the pointer (otherwise the tests fail)
    FHSSptr = 0;
    rngSeed(seed);

    // initialize the sequence array
    for (uint8_t i = 0; i < FHSS_SEQUENCE_CNT; i++)
    {
        if (i % FHSS_FREQ_CNT == 0) {
            FHSSsequence[i] = sync_channel;
        } else if (i % FHSS_FREQ_CNT == sync_channel) {
            FHSSsequence[i] = 0;
        } else {
            FHSSsequence[i] = i % FHSS_FREQ_CNT;
        }
    }

    for (uint8_t i=0; i < FHSS_SEQUENCE_CNT; i++)
    {
        // if it's not the sync channel
        if (i % FHSS_FREQ_CNT != 0)
        {
            uint8_t offset = (i / FHSS_FREQ_CNT) * FHSS_FREQ_CNT; // offset to start of current block
            uint8_t rand = rngN(FHSS_FREQ_CNT-1)+1; // random number between 1 and FHSS_FREQ_CNT

            // switch this entry and another random entry in the same block
            uint8_t temp = FHSSsequence[i];
            FHSSsequence[i] = FHSSsequence[offset+rand];
            FHSSsequence[offset+rand] = temp;
        }
    }

//    // output FHSS sequence
//    for (uint8_t i=0; i < FHSS_SEQUENCE_CNT; i++)
//    {
//        DBG("%u ",FHSSsequence[i]);
//        if (i % 10 == 9)
//            DBGCR;
//    }
//    DBGCR;
}
