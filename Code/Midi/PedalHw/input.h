//
// Created by ritwi on 2/15/2024.
//

#ifndef PEDALBOARD_INPUT_H
#define PEDALBOARD_INPUT_H

#include <stdbool.h>

struct pedalState {
    float adc0;
    float adc1;
    float adc2;
    float adc3;
    bool stomp0;
    bool stomp1;
};

int pedal_input_init();
int pedal_get_current_state(float* volts);

#endif //PEDALBOARD_INPUT_H
