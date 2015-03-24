/* 
 * File:    POEadc.c
 * Author:  Patrick O. Ehrmann (pst69@pst69.de)
 * License: Creative Commons Zero (https://creativecommons.org/publicdomain/zero/1.0/)
 *
 * Created on 2015-03-24
 * Description:
 *   ADC (Analog digital conversion) library
 * Major Changes:
 *   Version 0: alpha development
 */

#include "POEadc.h"

#ifdef APP_ADC_ID

void ADC_Initialize(void) {
    // init ADC module for polling 
    PLIB_ADC_Disable(APP_ADC_ID);
    // port inits
#ifdef APP_ADC1_INPUT_POS
    PLIB_PORTS_PinClear(APP_ADC1_PORTS_ID, APP_ADC1_PORT_CHANNEL, APP_ADC1_PIN);
    PLIB_PORTS_PinDirectionInputSet(APP_ADC1_PORTS_ID, APP_ADC1_PORT_CHANNEL, APP_ADC1_PIN);
    PLIB_PORTS_PinModeSelect(APP_ADC1_PORTS_ID, APP_ADC1_AIPIN, PORTS_PIN_MODE_ANALOG);
#endif // ifdef APP_ADC1_INPUT_POS
#ifdef APP_ADC2_INPUT_POS
    PLIB_PORTS_PinClear(APP_ADC2_PORTS_ID, APP_ADC2_PORT_CHANNEL, APP_ADC2_PIN);
    PLIB_PORTS_PinDirectionInputSet(APP_ADC2_PORTS_ID, APP_ADC2_PORT_CHANNEL, APP_ADC2_PIN);
    PLIB_PORTS_PinModeSelect(APP_ADC2_PORTS_ID, APP_ADC2_AIPIN, PORTS_PIN_MODE_ANALOG);
#endif // ifdef APP_ADC2_INPUT_POS
#ifdef APP_ADC3_INPUT_POS
    PLIB_PORTS_PinClear(APP_ADC3_PORTS_ID, APP_ADC3_PORT_CHANNEL, APP_ADC3_PIN);
    PLIB_PORTS_PinDirectionInputSet(APP_ADC3_PORTS_ID, APP_ADC3_PORT_CHANNEL, APP_ADC3_PIN);
    PLIB_PORTS_PinModeSelect(APP_ADC3_PORTS_ID, APP_ADC3_AIPIN, PORTS_PIN_MODE_ANALOG);
#endif // ifdef APP_ADC3_INPUT_POS
#ifdef APP_ADC4_INPUT_POS
    PLIB_PORTS_PinClear(APP_ADC4_PORTS_ID, APP_ADC4_PORT_CHANNEL, APP_ADC4_PIN);
    PLIB_PORTS_PinDirectionInputSet(APP_ADC4_PORTS_ID, APP_ADC4_PORT_CHANNEL, APP_ADC4_PIN);
    PLIB_PORTS_PinModeSelect(APP_ADC4_PORTS_ID, APP_ADC4_AIPIN, PORTS_PIN_MODE_ANALOG);
#endif // ifdef APP_ADC4_INPUT_POS
    // Reference range allways AVdd / AVss
    PLIB_ADC_VoltageReferenceSelect(APP_ADC_ID, ADC_REFERENCE_VDD_TO_AVSS);
    PLIB_ADC_StopInIdleDisable(APP_ADC_ID);
    /* Conversion clock */
    PLIB_ADC_ConversionClockSet(APP_ADC_ID, APP_ADC_CCLK_BASE, APP_ADC_CCLK_RATE);
    PLIB_ADC_ConversionTriggerSourceSelect(APP_ADC_ID, ADC_CONVERSION_TRIGGER_SAMP_CLEAR);
    /* preselect input negative (-reference) */
    PLIB_ADC_MuxChannel0InputNegativeSelect(APP_ADC_ID, ADC_MUX_A, ADC_INPUT_NEGATIVE_VREF_MINUS);
    /* Format */
    PLIB_ADC_ResultFormatSelect(APP_ADC_ID, ADC_RESULT_FORMAT_INTEGER_16BIT);
    PLIB_ADC_ResultBufferModeSelect(APP_ADC_ID, ADC_BUFFER_MODE_ONE_16WORD_BUFFER);
    /* Enable ADC */
    PLIB_ADC_Enable(APP_ADC_ID);
}

// start sampling of position
void ADC_StartSample(int pos) {
    PLIB_ADC_MuxChannel0InputNegativeSelect(APP_AIN_ADC_ID, ADC_MUX_A, ADC_INPUT_NEGATIVE_VREF_MINUS);
    switch (pos) {
        case 2:
            PLIB_ADC_MuxChannel0InputPositiveSelect(APP_AIN_ADC_ID, ADC_MUX_A, APP_AIN1_INPUT_POS);
            break;
        case 3:
            break;
        case 4:
            break;
        default:
            break;
    }
    PLIB_ADC_SamplingStart(APP_AIN_ADC_ID);
}

// start sample conversion
void ADC_ConvertSample(void) {
            PLIB_ADC_ConversionStart(APP_AIN_ADC_ID);
    
}

// wait on result
bool ADC_ResultIfReady(int *result) {
            if (PLIB_ADC_ConversionHasCompleted(APP_AIN_ADC_ID)) {
                appData.adcResult1 = PLIB_ADC_ResultGetByIndex(APP_AIN_ADC_ID, 0);
    
}

#endif //ifdef APP_ADC_ID
