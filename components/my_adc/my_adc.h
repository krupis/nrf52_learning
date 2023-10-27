#ifndef MY_ADC_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include "stdio.h"


void configure_adc();
int32_t ADC_read_mv1();
int32_t ADC_read_mv2();

int32_t ADC_read_mv(const struct adc_sequence* sequence);

extern struct adc_sequence sequence_7;
extern struct adc_sequence sequence_6;

#endif