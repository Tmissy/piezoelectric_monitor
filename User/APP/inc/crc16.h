#ifndef _CRC_CRC16_H
#define _CRC_CRC16_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus  */

uint16_t crc16(const char *buf, int len);
uint16_t crc16_con(const char *buf, int len, uint16_t base_crc);

#ifdef __cplusplus
}
#endif /* __cplusplus  */
#endif
