//
// Created by ritwi on 2/29/2024.
//
#include "iohandler.h"
#include <PicoLed.hpp>
extern "C"{
    #include "input.h"
}

#define LED_PIO pio0
#define LED_PIN 3
#define LED_LENGTH 6
#define LED_STATE_MACHINE 0

#define ASSUMED_MIN 500
#define ASSUMED_MAX 26000
#define MAX_DELTA 50

uint8_t colors[6] = {0, 51, 102, 153, 204, 255};
PicoLed::Color led_colors[6];
PicoLed::PicoLedController ledStrip = PicoLed::addLeds<PicoLed::WS2812B>(LED_PIO, LED_STATE_MACHINE, LED_PIN, LED_LENGTH, PicoLed::FORMAT_RGB);

float InverseLerp(float xx, float yy, float value)
{
    return std::clamp((value - xx)/(yy - xx),0.0f,1.0f);
}

uint16_t sat_subu16b(uint16_t x, uint16_t y)
{
    uint16_t res = x - y;
    res &= -(res <= x);

    return res;
}

int iohandler_init(){
    pedal_input_init();
    return 0;
}

uint16_t data[4] = {0,0,0,0};
uint16_t min_data[4] = {ASSUMED_MIN,ASSUMED_MIN,ASSUMED_MIN,ASSUMED_MIN};
uint16_t max_data[4] = {ASSUMED_MAX,ASSUMED_MAX,ASSUMED_MAX,ASSUMED_MAX};
int iohandler_get_current_state(float* volts,bool* stomps){
    pedal_get_current_state(data,stomps);
    for(int i=0;i<4;i++){
        min_data[i] = MIN(min_data[i],data[i]+MAX_DELTA);
        max_data[i] = MAX(max_data[i],sat_subu16b(data[i],MAX_DELTA));
        volts[i] = InverseLerp(max_data[i],min_data[i],data[i]);
        led_colors[i] = PicoLed::HSV(colors[i],255,(u_int8_t)std::lerp(0,255,volts[i]));
        ledStrip.setPixelColor(i,led_colors[i]);
    }
    for(int i=0;i<2;i++){
        led_colors[i+4] = PicoLed::HSV(colors[i],255,(u_int8_t)stomps[i]*255);
        ledStrip.setPixelColor(i+4,led_colors[i+4]);
    }
    ledStrip.show();
    return 0;
}

int iohandler_update_colors(uint8_t* newColors,u_int8_t brightness){
    for (int i = 0; i < 6; i++) {
        colors[i] = newColors[i];
    }
    ledStrip.setBrightness(brightness);
    return 0;
}

#include "iohandler.h"
