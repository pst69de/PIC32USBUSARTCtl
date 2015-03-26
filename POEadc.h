/* 
 * File:    POEadc.h
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-24
 * Description:
 *   ADC (Analog digital conversion) library
 *   This library uses polling instead of interrupts as simple ADC is not timing critical
 * Major Changes:
 *   Version 0: alpha development
 */

#ifndef POEADC_H
#define	POEADC_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "system_config/sk_pic32_mx/system_config.h"

#ifdef APP_ADC_ID

// Initialize ADC module of MCU
void ADC_Initialize(void);
// start sampling of position
void ADC_StartSample(int pos);
// start sample conversion
void ADC_ConvertSample(void);
// wait on result
bool ADC_ResultIfReady(int *result);

#endif // ifdef APP_ADC_ID

#ifdef	__cplusplus
}
#endif

#endif	/* POEADC_H */
