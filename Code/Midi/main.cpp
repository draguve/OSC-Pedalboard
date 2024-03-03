#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "PedalHw/iohandler.h"
#include <pico/unique_id.h>
#include <device/usbd.h>
#include <algorithm>
#include "pico/multicore.h"
#include "bsp/board.h"
#include "Utils/float_compare.h"
#include "PedalHw/midi_handler.h"

extern "C"{
    #include "dhcpserver/dhcpserver.h"
    #include "dnsserver/dnsserver.h"
}


#define UDP_PORT 4444
#define BEACON_MSG_LEN_MAX 127
#define BEACON_TARGET "255.255.255.255"
#define BEACON_INTERVAL_MS 1000

#define MIDI_EPSILON (1.0f/127.0f)

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

float current_volts[4] = {0, 0, 0, 0};
bool current_stomps[2] = {false,false};
float next_volts[4] =  {0, 0, 0, 0};
bool next_stomps[2] = {false,false};


float* current_volts_ptr = current_volts;
bool* current_stomps_ptr = current_stomps;
float* next_volts_ptr = next_volts;
bool* next_stomps_ptr = next_stomps;

bool midi_changed_pots[4] = {false,false,false,false};
bool midi_changed_stomps[2] = {false,false};

void core1_main(){
    iohandler_init();
    while (true) {
        iohandler_get_current_state(next_volts_ptr,next_stomps_ptr);
//        printf("%d %d A:%f B:%f C:%f D:%f\n",stomps[0],stomps[1],volts[0],volts[1],volts[2],volts[3]);
        is_changed(current_volts_ptr,next_volts_ptr,MIDI_EPSILON,midi_changed_pots,current_stomps_ptr,next_stomps_ptr,midi_changed_stomps);
        midi_task(next_volts_ptr,next_stomps_ptr,midi_changed_pots,midi_changed_stomps);
        std::swap(current_volts_ptr,next_volts_ptr);
        std::swap(current_stomps_ptr,next_stomps_ptr);
    }
}

bool repeating_timer_callback(struct repeating_timer *t) {
    tud_task();
    return true;
}

int main(void)
{
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        while(true){
            printf("Could not start wifi\n");
            sleep_ms(1000);
        }
    }

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
    cyw43_arch_enable_sta_mode();
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms("Draguve4", "pioneer123", CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect. starting ap mode\n");
        cyw43_arch_disable_sta_mode();
        cyw43_arch_enable_ap_mode(wifiname,uname,CYW43_AUTH_WPA2_AES_PSK);

        ip_addr_t gw;
        ip4_addr_t mask;
        IP4_ADDR(ip_2_ip4(&gw), 192, 168, 4, 1);
        IP4_ADDR(ip_2_ip4(&mask), 255, 255, 255, 0);

        // Start the dhcp server
        dhcp_server_t dhcp_server;
        dhcp_server_init(&dhcp_server, &gw, &mask);

        // Start the dns server
        dns_server_t dns_server;
        dns_server_init(&dns_server, &gw);

    } else {
        printf("Connected.\n");
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    while(true){
        sleep_ms(100);
    }
    cyw43_arch_deinit();
    return 0;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void){
//    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
}

// Invoked when device is unmounted
void tud_umount_cb(void){
//    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

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