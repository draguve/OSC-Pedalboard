//
// Created by ritwi on 3/2/2024.
//
#include <cmath>
#include "tusb.h"
#include "midi_handler.h"

uint8_t midi_locations_pot[] = {16,17,18,19};
uint8_t midi_locations_stomps[] = {20,21};

void midi_task(float* pots ,bool *stomp,bool* changedPots,bool *changedStomps){
    uint8_t const cable_num = 0; // MIDI jack associated with USB endpoint
    uint8_t const channel   = 0; // 0 for channel 1

    // The MIDI interface always creates input and output port/jack descriptors
    // regardless of these being used or not. Therefore incoming traffic should be read
    // (possibly just discarded) to avoid the sender blocking in IO
    uint8_t packet[4];
    while ( tud_midi_available() ) tud_midi_packet_read(packet);

    for(uint8_t i=0;i<4;i++){
        if(changedPots[i]){
            auto data = (uint8_t)std::lerp(0,127,pots[i]) ;
            uint8_t buffer[3] = { 0xB0 | channel, midi_locations_pot[i], data};
            tud_midi_stream_write(cable_num,buffer,3);
        }
    }
    for(uint8_t i=0;i<2;i++){
        if(changedStomps[i]){
            uint8_t data = stomp[i]*127;
            uint8_t buffer[3] = { 0xB0 | channel, midi_locations_stomps[i], data};
            tud_midi_stream_write(cable_num,buffer,3);
        }
    }
}

void midi_change_channel(uint8_t new_channel){

}