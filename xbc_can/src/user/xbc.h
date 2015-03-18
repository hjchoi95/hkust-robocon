#ifndef __XBC_H
#define __XBC_H

#include "stm32f10x.h" 
#include "can_protocol.h"
#include "usb_api.h"
#include "tft.h"
#include "ticks.h"

#define XBC_DATA_RAW_COUNT    20
#define XBC_DATA_COUNT        13

#define CAN_XBC_BASE                0x90
#define CAN_XBC_MB_TX_ID            0x100
#define CAN_XBC_LCD_IDLE_TIME_MS    1500
#define CAN_XBC_LCD_UPDATE_TRIVIAL_TIME             1

#define RGB565TORGB323(RGB565)  (((RGB565 >> 8) & 0xE0) | ((RGB565 >> 6) & 0x18) | ((RGB565 >> 2) & 0x07))
#define RGB323TORGB565(RGB323)  (((((RGB323 >> 5) & 0x07) * 0x1F / 0x07) << 11) | ((((RGB323 >> 3) & 0x03) * 0x3F / 0x03) << 5) | (((RGB323) & 0x07) * 0x1F / 0x07))

#define GRAY4TORGB565(GRAY4) ((GRAY4 << 12) | (GRAY4 << 7) | (GRAY4 << 1))

typedef struct {
  u16 color, bg_color;
  char text;
  
} XBC_LCD_DATA;

void xbc_rx_init(void);
u8 get_xbc_data(u8 i);
void xbc_tx_data(void);
void xbc_loop(void);
u8 xbc_rx_lcd_update(void);
u32 xbc_get_last_rx_time_ms(void);

#endif  /** __XBC_H **/
