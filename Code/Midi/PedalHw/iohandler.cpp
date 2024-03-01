//
// Created by ritwi on 2/29/2024.
//
#include "iohandler.h"
#include <PicoLed.hpp>
extern "C"{
    #include "input.h"
}

#define LED_PIO pio0
#define LED_PIN 10
#define LED_LENGTH 6
#define LED_STATE_MACHINE 0


int iohandler_init(){
    auto ledStrip = PicoLed::addLeds<PicoLed::WS2812B>(LED_PIO, LED_STATE_MACHINE, LED_PIN, LED_LENGTH, PicoLed::FORMAT_RGB);
    pedal_input_init();
    return 0;
}

int iohandler_get_current_state(float* volts,bool* stomps){
    pedal_get_current_state(volts,stomps);
    return 0;
}

int iohandler_update_colors(){
    return 0;
}

#include "iohandler.h"
