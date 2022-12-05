#ifndef __AT_PARSE_H
#define __AT_PARSE_H

#include <stdbool.h>
#include <stdint.h>

#define AT_DELIMITER "\r\n"
#define AT_END_CHAR  '\n'

typedef void (*at_recv_cb)(char *pdata,uint16_t data_len,void *arg);

bool at_parse_match_obj(char *pdata,uint16_t datalen);
bool at_parse_init(void);
bool at_parse_deinit(void); 
bool at_send_wait_reply(const char *cmd,bool delimiter,
                        char *right_prefix,char *fail_prefix,
                        char *reply_buf, uint16_t *reply_size,
                        uint32_t timeout);

bool at_send_no_reply(const char *data, uint16_t datalen, bool delimiter,uint32_t timeout);

bool at_register_callback(const char *prefix, at_recv_cb cb, void *arg);


//all cmd recv callback						
void at_signal_cb(char *pdata,uint16_t data_len,void *arg);
void at_net_con_cb(char *pdata,uint16_t data_len,void *arg);
void at_net_recv_cb(char *pdata,uint16_t data_len,void *arg);

#endif
