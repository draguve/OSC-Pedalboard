//
// Created by ritwi on 2/15/2024.
//

#ifndef PEDALBOARD_INPUT_H
#define PEDALBOARD_INPUT_H

#include <stdbool.h>

int pedal_input_init();
int pedal_get_current_state(float* volts,bool* stomps);

#endif //PEDALBOARD_INPUT_H
