//
// Created by ritwi on 3/2/2024.
//

#ifndef PEDALBOARD_MIDI_HANDLER_H
#define PEDALBOARD_MIDI_HANDLER_H
#include "pico.h"
void midi_task(float* pots ,bool *stomp,bool* changedPots,bool *changedStomps);
void midi_change_channel(uint8_t new_channel);
#endif //PEDALBOARD_MIDI_HANDLER_H
