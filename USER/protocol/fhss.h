#ifndef __FHSS_H_
#define __FHSS_H_

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

#if defined(Regulatory_Domain_AU_915) || defined(Regulatory_Domain_EU_868) || defined(Regulatory_Domain_FCC_915) || defined(Regulatory_Domain_AU_433) || defined(Regulatory_Domain_EU_433)
#include "SX127xDriver.h"
#elif Regulatory_Domain_ISM_2400
#include "sx1280.h"
#endif


#define RNG_MAX 0x7FFF


extern volatile uint8_t FHSSptr;
extern uint8_t NumOfFHSSfrequencies;
extern int32_t FreqCorrection;

#define FreqCorrectionMax ((int32_t)(100000/FREQ_STEP))
#define FreqCorrectionMin ((int32_t)(-100000/FREQ_STEP))

#define FREQ_HZ_TO_REG_VAL_900(freq) ((uint32_t)((double) freq / (double) SX127x_FREQ_STEP))
#define FREQ_HZ_TO_REG_VAL_24(freq) ((uint32_t)((double) freq / (double) SX1280_FREQ_STEP))




// The number of FHSS frequencies in the table
#define NR_FHSS_ENTRIES (sizeof(FHSSfreqsISM2400) / sizeof(uint32_t))

#define NR_SEQUENCE_ENTRIES 256
extern uint8_t FHSSsequence[NR_SEQUENCE_ENTRIES];
void FHSSrandomiseFHSSsequence(const uint8_t UID[]);

void FHSSsetCurrIndex(uint8_t value);
uint8_t FHSSgetCurrIndex(void);
uint32_t GetInitialFreq(void);
uint32_t FHSSgetNextFreq(void);

#endif
