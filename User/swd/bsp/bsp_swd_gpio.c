/*
*********************************************************************************************************
*
*	模块名称 : ST7789 TFT SPI接口驱动程序
*	文件名称 : bsp_tft_st7789.c
*	版    本 : V1.0
*	说    明 : SPI接口，显示驱动IC为ST7789，分辨率为240*240,1.3寸ISP
*	修改记录 :
*		版本号  日期       作者    说明
*		V1.0	2018-12-06 armfly 
*		V1.1	2019-03-25 armfly 软件SPI，优化执行速度
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

// SW Macros

#define PIN_SWCLK_SET PIN_SWCLK_TCK_SET
#define PIN_SWCLK_CLR PIN_SWCLK_TCK_CLR

#define SW_CLOCK_CYCLE()                \
  PIN_SWCLK_CLR();                      \
  PIN_DELAY();                          \
  PIN_SWCLK_SET();                      \
  PIN_DELAY()

#define SW_WRITE_BIT(bit)               \
  PIN_SWDIO_OUT(bit);                   \
  PIN_SWCLK_CLR();                      \
  PIN_DELAY();                          \
  PIN_SWCLK_SET();                      \
  PIN_DELAY()

#define SW_READ_BIT(bit)                \
  PIN_SWCLK_CLR();                      \
  PIN_DELAY();                          \
  bit = PIN_SWDIO_IN();                 \
  PIN_SWCLK_SET();                      \
  PIN_DELAY()

#define PIN_DELAY() PIN_DELAY_SLOW(DAP_Data.clock_delay)


// Generate SWJ Sequence
//   count:  sequence bit count
//   data:   pointer to sequence bit data
//   return: none
void SWJ_Sequence (uint32_t count, const uint8_t *data) {
  uint32_t val;
  uint32_t n;

  val = 0U;
  n = 0U;
  while (count--) {
    if (n == 0U) {
      val = *data++;
      n = 8U;
    }
    if (val & 1U) {
      PIN_SWDIO_TMS_SET();
    } else {
      PIN_SWDIO_TMS_CLR();
    }
    SW_CLOCK_CYCLE();
    val >>= 1;
    n--;
  }
}

// SWD Transfer I/O
//   request: A[3:2] RnW APnDP
//   data:    DATA[31:0]
//   return:  ACK[2:0]
#define SWD_TransferFunction(speed)     /**/                                    \
uint8_t SWD_Transfer##speed (uint32_t request, uint32_t *data) {                \
  uint32_t ack;                                                                 \
  uint32_t bit;                                                                 \
  uint32_t val;                                                                 \
  uint32_t parity;                                                              \
                                                                                \
  uint32_t n;                                                                   \
                                                                                \
  /* Packet Request */                                                          \
  parity = 0U;                                                                  \
  SW_WRITE_BIT(1U);                     /* Start Bit */                         \
  bit = request >> 0;                                                           \
  SW_WRITE_BIT(bit);                    /* APnDP Bit */                         \
  parity += bit;                                                                \
  bit = request >> 1;                                                           \
  SW_WRITE_BIT(bit);                    /* RnW Bit */                           \
  parity += bit;                                                                \
  bit = request >> 2;                                                           \
  SW_WRITE_BIT(bit);                    /* A2 Bit */                            \
  parity += bit;                                                                \
  bit = request >> 3;                                                           \
  SW_WRITE_BIT(bit);                    /* A3 Bit */                            \
  parity += bit;                                                                \
  SW_WRITE_BIT(parity);                 /* Parity Bit */                        \
  SW_WRITE_BIT(0U);                     /* Stop Bit */                          \
  SW_WRITE_BIT(1U);                     /* Park Bit */                          \
                                                                                \
  /* Turnaround */                                                              \
  PIN_SWDIO_OUT_DISABLE();                                                      \
  for (n = DAP_Data.swd_conf.turnaround; n; n--) {                              \
    SW_CLOCK_CYCLE();                                                           \
  }                                                                             \
                                                                                \
  /* Acknowledge response */                                                    \
  SW_READ_BIT(bit);                                                             \
  ack  = bit << 0;                                                              \
  SW_READ_BIT(bit);                                                             \
  ack |= bit << 1;                                                              \
  SW_READ_BIT(bit);                                                             \
  ack |= bit << 2;                                                              \
                                                                                \
  if (ack == DAP_TRANSFER_OK) {         /* OK response */                       \
    /* Data transfer */                                                         \
    if (request & DAP_TRANSFER_RnW) {                                           \
      /* Read data */                                                           \
      val = 0U;                                                                 \
      parity = 0U;                                                              \
      for (n = 32U; n; n--) {                                                   \
        SW_READ_BIT(bit);               /* Read RDATA[0:31] */                  \
        parity += bit;                                                          \
        val >>= 1;                                                              \
        val  |= bit << 31;                                                      \
      }                                                                         \
      SW_READ_BIT(bit);                 /* Read Parity */                       \
      if ((parity ^ bit) & 1U) {                                                \
        ack = DAP_TRANSFER_ERROR;                                               \
      }                                                                         \
      if (data) { *data = val; }                                                \
      /* Turnaround */                                                          \
      for (n = DAP_Data.swd_conf.turnaround; n; n--) {                          \
        SW_CLOCK_CYCLE();                                                       \
      }                                                                         \
      PIN_SWDIO_OUT_ENABLE();                                                   \
    } else {                                                                    \
      /* Turnaround */                                                          \
      for (n = DAP_Data.swd_conf.turnaround; n; n--) {                          \
        SW_CLOCK_CYCLE();                                                       \
      }                                                                         \
      PIN_SWDIO_OUT_ENABLE();                                                   \
      /* Write data */                                                          \
      val = *data;                                                              \
      parity = 0U;                                                              \
      for (n = 32U; n; n--) {                                                   \
        SW_WRITE_BIT(val);              /* Write WDATA[0:31] */                 \
        parity += val;                                                          \
        val >>= 1;                                                              \
      }                                                                         \
      SW_WRITE_BIT(parity);             /* Write Parity Bit */                  \
    }                                                                           \
    /* Idle cycles */                                                           \
    n = DAP_Data.transfer.idle_cycles;                                          \
    if (n) {                                                                    \
      PIN_SWDIO_OUT(0U);                                                        \
      for (; n; n--) {                                                          \
        SW_CLOCK_CYCLE();                                                       \
      }                                                                         \
    }                                                                           \
    PIN_SWDIO_OUT(1U);                                                          \
    return ((uint8_t)ack);                                                      \
  }                                                                             \
                                                                                \
  if ((ack == DAP_TRANSFER_WAIT) || (ack == DAP_TRANSFER_FAULT)) {              \
    /* WAIT or FAULT response */                                                \
    if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) != 0U)) { \
      for (n = 32U+1U; n; n--) {                                                \
        SW_CLOCK_CYCLE();               /* Dummy Read RDATA[0:31] + Parity */   \
      }                                                                         \
    }                                                                           \
    /* Turnaround */                                                            \
    for (n = DAP_Data.swd_conf.turnaround; n; n--) {                            \
      SW_CLOCK_CYCLE();                                                         \
    }                                                                           \
    PIN_SWDIO_OUT_ENABLE();                                                     \
    if (DAP_Data.swd_conf.data_phase && ((request & DAP_TRANSFER_RnW) == 0U)) { \
      PIN_SWDIO_OUT(0U);                                                        \
      for (n = 32U+1U; n; n--) {                                                \
        SW_CLOCK_CYCLE();               /* Dummy Write WDATA[0:31] + Parity */  \
      }                                                                         \
    }                                                                           \
    PIN_SWDIO_OUT(1U);                                                          \
    return ((uint8_t)ack);                                                      \
  }                                                                             \
                                                                                \
  /* Protocol error */                                                          \
  for (n = DAP_Data.swd_conf.turnaround + 32U + 1U; n; n--) {                   \
    SW_CLOCK_CYCLE();                   /* Back off data phase */               \
  }                                                                             \
  PIN_SWDIO_OUT_ENABLE();                                                       \
  PIN_SWDIO_OUT(1U);                                                            \
  return ((uint8_t)ack);                                                        \
}

#undef  PIN_DELAY
#define PIN_DELAY() PIN_DELAY_FAST()
SWD_TransferFunction(Fast);

#undef  PIN_DELAY
#define PIN_DELAY() PIN_DELAY_SLOW(DAP_Data.clock_delay)
SWD_TransferFunction(Slow);


// SWD Transfer I/O
//   request: A[3:2] RnW APnDP
//   data:    DATA[31:0]
//   return:  ACK[2:0]
uint8_t  SWD_Transfer(uint32_t request, uint32_t *data) {
  if (DAP_Data.fast_clock) {
    return SWD_TransferFast(request, data);
  } else {
    return SWD_TransferSlow(request, data);
  }
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
