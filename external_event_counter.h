/*
 * external_event_counter.h
 *
 *  Created on: Jan 2, 2020
 *      Author: Naveed
 */

#ifndef EXTERNAL_EVENT_COUNTER_H_
#define EXTERNAL_EVENT_COUNTER_H_

// #define TC_INST		TC4->COUNT16	// The instance used for the TC
#define TC_INST		TCC0	// The instance used for the TC

#define GCLK_N	6	// GCLK
#define PCH_M		25	// Peripheral Channel

void EE_Cntr_Init(void);
unsigned int get_count(void);

#endif /* EXTERNAL_EVENT_COUNTER_H_ */
