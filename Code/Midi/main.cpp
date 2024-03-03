#include <cstdio>
#include "pico/stdlib.h"
#include <cstring>
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
#include "PedalHw/osc_handler.h"
#include "pedal_settings.h"

extern "C"{
    #include "dhcpserver/dhcpserver.h"
    #include "dnsserver/dnsserver.h"
}


#define UDP_PORT 4444
#define BEACON_MSG_LEN_MAX 127
#define BEACON_TARGET "255.255.255.255"
#define BEACON_INTERVAL_MS 1000

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

bool is_connected = false;
void core1_main(){
    iohandler_init();
    osc_init();
    while (true) {
        iohandler_get_current_state(next_volts_ptr,next_stomps_ptr);
        is_changed(current_volts_ptr,next_volts_ptr,MIDI_EPSILON,midi_changed_pots,current_stomps_ptr,next_stomps_ptr,midi_changed_stomps);
        midi_task(next_volts_ptr,next_stomps_ptr,midi_changed_pots,midi_changed_stomps);
        if(is_connected){
            is_changed(current_volts_ptr,next_volts_ptr,OSC_EPSILON,midi_changed_pots,current_stomps_ptr,next_stomps_ptr,midi_changed_stomps);
            osc_task(next_volts_ptr,next_stomps_ptr,midi_changed_pots,midi_changed_stomps);
        }
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
    if (cyw43_arch_wifi_connect_timeout_ms("Draguve4", "pioneer123", CYW43_AUTH_WPA2_MIXED_PSK, 30000)) {
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
    is_connected = true;
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