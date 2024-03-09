//
// Created by ritwi on 2/15/2024.
//
//#include ""
#include "input.h"
#include "pico/stdlib.h"
#include <ads1115.h>
#include <stdio.h>

#define stomp0Pin 12
#define stomp1Pin 11

#define I2C_PORT i2c1
#define I2C_FREQ 400000
#define ADS1115_I2C_ADDR 0x49
const uint8_t SDA_PIN = 18;
const uint8_t SCL_PIN = 19;

struct ads1115_adc adc0;
struct ads1115_adc adc1;
struct ads1115_adc adc2;
struct ads1115_adc adc3;

void setup_adc(enum ads1115_mux_t mux, ads1115_adc_t *adc){
    ads1115_init(I2C_PORT, ADS1115_I2C_ADDR, adc);
    ads1115_set_input_mux(mux, adc);
    ads1115_set_pga(ADS1115_PGA_4_096,adc);
    ads1115_set_data_rate(ADS1115_RATE_860_SPS, adc);
    ads1115_set_operating_mode(ADS1115_MODE_SINGLE_SHOT,adc);
    ads1115_write_config(adc,true);
}

int pedal_input_init(){
    // Initialise I2C
    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    gpio_init(stomp0Pin);
    gpio_init(stomp1Pin);
    gpio_set_dir(stomp0Pin,GPIO_IN);
    gpio_set_dir(stomp1Pin,GPIO_IN);

    // Initialise ADC
    setup_adc(ADS1115_MUX_SINGLE_0,&adc0);
    setup_adc(ADS1115_MUX_SINGLE_1,&adc1);
    setup_adc(ADS1115_MUX_SINGLE_2,&adc2);
    setup_adc(ADS1115_MUX_SINGLE_3,&adc3);
    return 0;
}

uint16_t adc_value;
uint16_t adc_value1;
uint16_t adc_value2;
uint16_t adc_value3;

int pedal_get_current_state(uint16_t* volts,bool* stomps){
    ads1115_read_adc(&adc_value, &adc0);
    volts[0] = adc_value;
    ads1115_read_adc(&adc_value1, &adc1);
    volts[1] = adc_value1;
    ads1115_read_adc(&adc_value2, &adc2);
    volts[2] = adc_value2;
    ads1115_read_adc(&adc_value3, &adc3);
    volts[3] = adc_value3;
    stomps[0] = gpio_get(stomp1Pin);
    stomps[1] = gpio_get(stomp0Pin);
}