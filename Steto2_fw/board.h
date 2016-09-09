/*
 * board.h
 *
 *  Created on: 12 сент. 2015 г.
 *      Author: Kreyl
 */

#pragma once

#include <inttypes.h>

// ==== General ====
#define BOARD_NAME          "Steto2_1"
#define APP_NAME            "Steto2_1"

// MCU type as defined in the ST header.
#define STM32F030x6

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ     12000000

#define SYS_TIM_CLK         (Clk.APBFreqHz)
#define ADC_REQUIRED        TRUE

#if 1 // ========================== GPIO =======================================
// UART
#define UART_GPIO       GPIOA
#define UART_TX_PIN     9
#define UART_RX_PIN     10
#define UART_AF         AF1 // for USART1 @ GPIOA

// LED
//#define LED_EN_PIN      { GPIOB, 2, omPushPull }
//#define LED_R_PIN       { GPIOB, 1, TIM3, 4, invInverted, omOpenDrain, 255 }
//#define LED_G_PIN       { GPIOB, 0, TIM3, 3, invInverted, omOpenDrain, 255 }
//#define LED_B_PIN       { GPIOB, 5, TIM3, 2, invInverted, omOpenDrain, 255 }

// Buttons
#define BTN_UP_PIN      GPIOA, 0, pudPullUp
#define BTN_DOWN_PIN    GPIOA, 1, pudPullUp

// Pwr pin
#define PWR_PIN         GPIOA, 2, omPushPull
#endif // GPIO

#if 1 // ========================= Timer =======================================
#endif // Timer

#if 1 // ========================== USART ======================================
#define UART            USART1
#define UART_TX_REG     UART->TDR
#define UART_RX_REG     UART->RDR
#endif

#if ADC_REQUIRED // ======================= Inner ADC ==========================
// Clock divider: clock is generated from the APB2
#define ADC_CLK_DIVIDER		adcDiv2

// ADC channels
//#define BAT_CHNL 	        1

#define ADC_VREFINT_CHNL    17  // All 4xx, F072/F030 and L151 devices. Do not change.
#define ADC_CHANNELS        { 3, ADC_VREFINT_CHNL }
#define ADC_CHANNEL_CNT     2   // Do not use countof(AdcChannels) as preprocessor does not know what is countof => cannot check
#define ADC_SAMPLE_TIME     ast96Cycles
#define ADC_SAMPLE_CNT      8   // How many times to measure every channel

#define ADC_SEQ_LEN         (ADC_SAMPLE_CNT * ADC_CHANNEL_CNT)

#endif

#if 1 // =========================== DMA =======================================
#define STM32_DMA_REQUIRED  TRUE
// ==== Uart ====
#define UART_DMA_TX     STM32_DMA1_STREAM2
#define UART_DMA_RX     STM32_DMA1_STREAM3
#define UART_DMA_CHNL   0   // Dummy

#if ADC_REQUIRED
#define ADC_DMA         STM32_DMA1_STREAM1
#define ADC_DMA_MODE    STM32_DMA_CR_CHSEL(0) |   /* dummy */ \
                        DMA_PRIORITY_LOW | \
                        STM32_DMA_CR_MSIZE_HWORD | \
                        STM32_DMA_CR_PSIZE_HWORD | \
                        STM32_DMA_CR_MINC |       /* Memory pointer increase */ \
                        STM32_DMA_CR_DIR_P2M |    /* Direction is peripheral to memory */ \
                        STM32_DMA_CR_TCIE         /* Enable Transmission Complete IRQ */
#endif // ADC

#endif // DMA
