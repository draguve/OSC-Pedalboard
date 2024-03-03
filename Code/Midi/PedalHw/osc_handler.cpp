//
// Created by ritwi on 3/3/2024.
//

#include <cstring>
#include "osc_handler.h"
#include "pico/time.h"
#include "string"
#include "lwip/udp.h"

#define MAX_BUFF_LEN 512

char buffer[MAX_BUFF_LEN];
ip4_addr target_server;
u_int16_t target_port;
struct udp_pcb *pcb;


std::string addresses_pots[] = {
        "/Pot1",
        "/Pot2",
        "/Pot3",
        "/Pot4"
};

std::string addresses_stomps[] = {
        "/Stomp1",
        "/Stomp2",
};

void osc_init() {
    pcb = udp_new();
    IP4_ADDR(ip_2_ip4(&target_server), 255, 255, 255, 255);
    target_port = 8000;
}

void osc_task(float *pots, bool *stomp, bool *changedPots, bool *changedStomps) {
    tosc_bundle bundle;
    bool isEmpty = true;
    u_int64_t timetag = get_absolute_time()._private_us_since_boot;
    tosc_writeBundle(&bundle, timetag, buffer, sizeof(buffer));
    for (uint8_t i = 0; i < 4; i++) {
        if (changedPots[i]) {
            tosc_writeNextMessage(&bundle, addresses_pots[i].c_str(), "f", pots[i]);
            isEmpty = false;
        }
    }
    for (uint8_t i = 0; i < 2; i++) {
        if (changedStomps[i]) {
            tosc_writeNextMessage(&bundle, addresses_stomps[i].c_str(), "f", 1.0f * stomp[i]);
            isEmpty = false;
        }
    }
    if (!isEmpty) {
        send_bundle(bundle);
    }
}

void change_server_ip(ip4_addr addr, uint16_t port) {
    target_server = addr;
    target_port = port;
}

void send_bundle(tosc_bundle bundle) {
    struct pbuf *lwip_buf = pbuf_alloc(PBUF_TRANSPORT, tosc_getBundleLength(&bundle), PBUF_RAM);
    pbuf_take(lwip_buf, bundle.buffer, tosc_getBundleLength(&bundle));
    err_t er = udp_sendto(pcb, lwip_buf, &target_server, target_port);
    pbuf_free(lwip_buf);
    if (er != ERR_OK) {
        printf("Failed to send UDP packet! error=%d", er);
    }
}