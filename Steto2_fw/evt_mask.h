/*
 * evt_mask.h
 *
 *  Created on: Apr 12, 2013
 *      Author: g.kruglov
 */

#pragma once

// Event masks
#define EVT_UART_NEW_CMD    EVENT_MASK(1)
#define EVT_BUTTONS         EVENT_MASK(2)
#define EVT_OFF             EVENT_MASK(3)
#define EVT_LED_SEQ_END     EVENT_MASK(5)

#define EVT_PWR_POLL        EVENT_MASK(6)

// Adc
#define EVT_SAMPLING        EVENT_MASK(24)
#define EVT_ADC_DONE        EVENT_MASK(25)

// Eternal
#define EVT_EVERY_SECOND    EVENT_MASK(31)
