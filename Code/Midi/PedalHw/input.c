//
// Created by ritwi on 2/15/2024.
//
//#include ""
#include "input.h"
#include "pico/stdlib.h"
#include <ads1115.h>

#define I2C_PORT i2c1
#define I2C_FREQ 400000
#define ADS1115_I2C_ADDR 0x48
const uint8_t SDA_PIN = 18;
const uint8_t SCL_PIN = 19;

struct ads1115_adc adc0;
uint16_t singleShotAdc[4] = {ADS1115_MUX_SINGLE_0,ADS1115_MUX_SINGLE_1,ADS1115_MUX_SINGLE_2,ADS1115_MUX_SINGLE_3};

int pedal_input_init(){
    // Initialise I2C
    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Initialise ADC
    ads1115_init(I2C_PORT, ADS1115_I2C_ADDR, &adc0);
    ads1115_set_input_mux(ADS1115_MUX_SINGLE_0, &adc0);
    ads1115_set_data_rate(ADS1115_RATE_860_SPS, &adc0);
    ads1115_set_operating_mode(ADS1115_MODE_SINGLE_SHOT,&adc0);
    ads1115_write_config(&adc0);

    return 0;
}

uint16_t adc_value;
int pedal_get_current_state(float* volts){
    for(int i=0;i<4;i++){
        ads1115_set_input_mux(singleShotAdc[i],&adc0);
        ads1115_read_adc(&adc_value, &adc0);
        volts[i] = ads1115_raw_to_volts(adc_value,&adc0);
    }
    return 0;
}