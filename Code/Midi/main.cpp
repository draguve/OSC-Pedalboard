#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "PedalHw/iohandler.h"
#include <pico/unique_id.h>
#include <device/usbd.h>
#include "pico/multicore.h"
#include "bsp/board.h"

#define UDP_PORT 4444
#define BEACON_MSG_LEN_MAX 127
#define BEACON_TARGET "255.255.255.255"
#define BEACON_INTERVAL_MS 1000


void run_udp_beacon() {
    struct udp_pcb *pcb = udp_new();

    ip_addr_t addr;
    ipaddr_aton(BEACON_TARGET, &addr);

    int counter = 0;
    while (true) {
        struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, BEACON_MSG_LEN_MAX + 1, PBUF_RAM);
        char *req = (char *) p->payload;
        memset(req, 0, BEACON_MSG_LEN_MAX + 1);
        snprintf(req, BEACON_MSG_LEN_MAX, "%d\n", counter);
        err_t er = udp_sendto(pcb, p, &addr, UDP_PORT);
        pbuf_free(p);
        if (er != ERR_OK) {
            printf("Failed to send UDP packet! error=%d", er);
        } else {
            printf("Sent packet %d\n", counter);
            counter++;
        }

        sleep_ms(BEACON_INTERVAL_MS);
    }
}

void core1_main(){
    iohandler_init();
    float volts[4] = {0, 0, 0, 0};
    bool stomps[2] = {false,false};
    while (true) {
        iohandler_get_current_state(volts,stomps);
        printf("%d %d A:%f B:%f C:%f D:%f\n",stomps[0],stomps[1],volts[0],volts[1],volts[2],volts[3]);
        busy_wait_ms(10);
    }
}

bool repeating_timer_callback(struct repeating_timer *t) {
    tud_task();
    return true;
}

/*------------- MAIN -------------*/
int main(void)
{
    board_init();
    tud_init(BOARD_TUD_RHPORT);
    stdio_init_all();
    struct repeating_timer timer;
    add_repeating_timer_ms(10, repeating_timer_callback, NULL, &timer);

    multicore_launch_core1(core1_main);

    char uname[40];
    char wifiname[40];
    pico_get_unique_board_id_string(uname,10);
    sprintf(wifiname,"Pedal %s",uname);

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        while(true){
            printf("Could not start wifi\n");
            sleep_ms(1000);
        }
    }
    cyw43_arch_enable_sta_mode();
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms("Draguve4", "pioneer123", CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect. starting ap mode\n");
        cyw43_arch_disable_sta_mode();
        cyw43_arch_enable_ap_mode(wifiname,uname,CYW43_AUTH_WPA2_AES_PSK);
    } else {
        printf("Connected.\n");
    }
    while(true){
//        tud_task();
        sleep_ms(100);
    }
    cyw43_arch_deinit();
    return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void){}

// Invoked when device is unmounted
void tud_umount_cb(void){}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void){}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    (void)itf;
    (void)rts;

    if (dtr) {
//        tud_cdc_write_str("Connected\n");
    }
}