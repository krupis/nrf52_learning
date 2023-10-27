#include "my_adc.h"

#define ADC_NODE DT_PHANDLE(DT_PATH(zephyr_user), io_channels)

#define ADC_RESOLUTION 12
#define ADC_GAIN ADC_GAIN_1_6
#define ADC_REFERENCE ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME_DEFAULT
const struct device *dev_adc;

uint16_t sample_buffer7;
struct adc_sequence sequence_7;
#define ADC_CHANNEL7 DT_IO_CHANNELS_INPUT_BY_IDX(DT_PATH(zephyr_user), 0)

uint16_t sample_buffer6;
struct adc_sequence sequence_6;
#define ADC_CHANNEL6 DT_IO_CHANNELS_INPUT_BY_IDX(DT_PATH(zephyr_user), 1)

#define LOG_LEVEL 4
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(my_adc);

void configure_adc()
{
    dev_adc = DEVICE_DT_GET(ADC_NODE);
    if (!device_is_ready(dev_adc))
    {
        printf("ADC device not ready\n");
        return;
    }
    printf("ADC device ready\n");

    // CONFIGURE AIN7
    struct adc_channel_cfg channel7_cfg = {
        .gain = ADC_GAIN,
        .reference = ADC_REFERENCE,
        .acquisition_time = ADC_ACQUISITION_TIME,
        .channel_id = ADC_CHANNEL7,
        .differential = 0};

#ifdef CONFIG_ADC_NRFX_SAADC
    channel7_cfg.input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0 + ADC_CHANNEL7;
#endif
    adc_channel_setup(dev_adc, &channel7_cfg);
    sequence_7.channels = BIT(ADC_CHANNEL7);
    sequence_7.buffer = &sample_buffer7;
    sequence_7.buffer_size = sizeof(sample_buffer7);
    sequence_7.resolution = ADC_RESOLUTION;

    // CONFIGURE AIN6
    struct adc_channel_cfg channel6_cfg = {
        .gain = ADC_GAIN,
        .reference = ADC_REFERENCE,
        .acquisition_time = ADC_ACQUISITION_TIME,
        .channel_id = ADC_CHANNEL6,
        .differential = 0};

#ifdef CONFIG_ADC_NRFX_SAADC
    channel6_cfg.input_positive = SAADC_CH_PSELP_PSELP_AnalogInput0 + ADC_CHANNEL6;
#endif
    adc_channel_setup(dev_adc, &channel6_cfg);
    sequence_6.channels = BIT(ADC_CHANNEL6);
    sequence_6.buffer = &sample_buffer6;
    sequence_6.buffer_size = sizeof(sample_buffer6);
    sequence_6.resolution = ADC_RESOLUTION;
}

int32_t ADC_read_mv1()
{
    int err = adc_read(dev_adc, &sequence_7);
    if (err != 0)
    {
        printk("ADC reading failed with error %d\n", err);
        return -1;
    }
    else
    {
        printk("ADC raw reading: %d\n", sample_buffer7);
        int32_t mv_value = (int32_t)sample_buffer7;

        int err2 = adc_raw_to_millivolts(adc_ref_internal(dev_adc), ADC_GAIN, ADC_RESOLUTION, &mv_value);
        if (err2 < 0)
        {
            return -1;
        }
        else
        {
            return mv_value;
        }
    }
}

int32_t ADC_read_mv2()
{
    int err = adc_read(dev_adc, &sequence_6);
    if (err != 0)
    {
        printk("ADC reading failed with error %d\n", err);
        return -1;
    }
    else
    {
        printk("ADC raw reading: %d\n", sample_buffer6);
        int32_t mv_value = (int32_t)sample_buffer6;

        int err2 = adc_raw_to_millivolts(adc_ref_internal(dev_adc), ADC_GAIN, ADC_RESOLUTION, &mv_value);
        if (err2 < 0)
        {
            return -1;
        }
        else
        {
            return mv_value;
        }
    }
}

int32_t ADC_read_mv(const struct adc_sequence *sequence)
{
    int err = adc_read(dev_adc, sequence);
    if (err != 0)
    {
        printk("ADC reading failed with error %d\n", err);
        return -1;
    }
    else
    {
        int *p = sequence->buffer;
        int32_t test_value = (int32_t)*p;
        printf("my value test = %u \n ", test_value);
        int32_t mv_value = (int32_t)test_value;

        int err2 = adc_raw_to_millivolts(adc_ref_internal(dev_adc), ADC_GAIN, ADC_RESOLUTION, &mv_value);
        if (err2 < 0)
        {
            return -1;
        }
        else
        {
            return mv_value;
        }
    }
}