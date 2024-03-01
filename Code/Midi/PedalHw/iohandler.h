//
// Created by ritwi on 2/29/2024.
//

#ifndef PEDALBOARD_IOHANDLER_H
#define PEDALBOARD_IOHANDLER_H

int iohandler_init();
int iohandler_get_current_state(float* volts,bool* stomps);
int iohandler_update_colors();

#endif //PEDALBOARD_IOHANDLER_H
