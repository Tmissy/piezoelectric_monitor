#ifndef __DEV_SIM7020_H
#define __DEV_SIM7020_H

#include "at_parse.h"
#include "usart_config.h"

//#define REMOTE_IP           "103.46.128.49"
//#define REMOTE_PORT         43822

//#define REMOTE_IP           "8.140.1.18"
#define REMOTE_IP           "47.93.190.54"
#define REMOTE_PORT         6600

//#define REMOTE_IP           "121.36.220.210"
//#define REMOTE_PORT         9000

//#define REMOTE_IP           "183.230.174.137"
//#define REMOTE_PORT         6065

#define NB_SLEEP_800nA      4
#define NB_SLEEP_8uA        3
#define NB_SLEEP_22uA       2
#define NB_AUTO_PSM         1

extern char remote_ip[20] ;
extern uint16_t remote_port ;

bool sim7020_init(void);
bool sim7020_start_udp_tcp(bool ls_udp, char* remote_ip, uint16_t remote_port);
bool sim7020_send_udp_tcp(char* pdata, uint32_t data_size);
bool sim7020_close_udp_tcp(void);
bool sim7020_sleep(uint8_t sleep_depth);
bool sim7020_pwr_on(void);
bool sim7020_pwr_off(void);
bool sim7020_check_net(void);
bool sim7020_reboot(void);
bool sim7020_wakeup(void);
void sim7020_test(void);
void sim7020_handle(void);
bool sim7020_set_apn(uint8_t apn_mode);
bool sim7020_ls_idel(void);
bool sim7020_set_power(bool ls_sleep);
uint8_t sim7020_get_idel(void);
bool sim7020_loss_handle(void);

#endif
