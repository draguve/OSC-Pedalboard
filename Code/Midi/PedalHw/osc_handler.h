//
// Created by ritwi on 3/3/2024.
//

#ifndef PEDALBOARD_OSC_HANDLER_H
#define PEDALBOARD_OSC_HANDLER_H
#include "lwip/udp.h"
#include "tinyosc.h"

void osc_init();
void osc_task(float* pots ,bool *stomp,bool* changedPots,bool *changedStomps);
void change_server_ip(ip4_addr addr,uint16_t port);
void send_bundle(tosc_bundle bundle);
#endif //PEDALBOARD_OSC_HANDLER_H
