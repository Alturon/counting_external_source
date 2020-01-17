/*
 * external_event_counter.c
 *
 *  Created on: Jan 2, 2020
 *      Author: Naveed
 */

#include "external_event_counter.h"

#include "board.h"
#include "cpu.h"

#include "periph_conf.h"
#include "periph/gpio.h"

#define GPIO_DIAG	GPIO_PIN(PA, 22) // Clock source pin

void ee_gpio_config(void){
	// config the gpio to be used for reading the external clock source.
	// setup GCLK_IO[1] (PA15) as the source (input) | Function Group H

	// Enable Bus Clock
	// 		I suppose there is no need for this as the peripheral in
	//		bridge A group have this already enabled the bus clock.
	MCLK->APBAMASK.reg |= MCLK_APBAMASK_PORT;

	gpio_init_mux(GPIO_DIAG, GPIO_MUX_H);
	gpio_init(GPIO_DIAG, GPIO_IN);

	return;
}

void ee_gclk_config(void){
	// Pre-enabling config

	GCLK->GENCTRL[GCLK_N].reg = 0; // disable gclk[N]
	GCLK->GENCTRL[GCLK_N].reg = GCLK_GENCTRL_GENEN | GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_SRC_GCLKIN_Val;
	while(GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL_GCLK6);

	GCLK->PCHCTRL[PCH_M].reg = 0; // disable channel[M]
	GCLK->PCHCTRL[PCH_M].reg = GCLK_PCHCTRL_GEN_GCLK6 | GCLK_PCHCTRL_CHEN;
	while(GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL_GCLK6);

	return;
}

void ee_cntr_config(void){
	// Disable TC
	// - reset ENABLE bit in CTRLA, as the following registers are mostly protected.
	TC_INST->CTRLA.reg &= ~TC_CTRLA_ENABLE;

	// Pre-enabling setup
	// 1 Enable TC bus clock CLK_TCx_APB
	// MCLK->APBDMASK.reg |= MCLK_APBDMASK_TC4;
	MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC0;
	// MCLK->APBDMASK.bit.TC4_ = 1;
	// MCLK->APBCMASK.bit.TC1_ = 1;

	// 2 Select mode (8, 16 or 32: default 16) in CTRLA.MODE
	TC_INST->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;

	// 3 Select one wave generation in WAVE.WAVEGEN
	TC_INST->WAVE.reg |= TC_WAVE_WAVEGEN_NFRQ;

	// 4 If desired setup prescaler in CTRLA.PRESCALER
	TC_INST->CTRLA.reg &= ~TC_CTRLA_PRESCALER_Msk; // div1 = 0x0

	// 5 If desired setup one-shot operation in CTRLBSET.ONESHOT
	TC_INST->CTRLBCLR.reg |= TC_CTRLBCLR_ONESHOT; // 0 for disabling one-shot

	// 6 Select count up or down, in CTRLBSET.DIR: 0 for up, 1 for down
	TC_INST->CTRLBCLR.reg |= TC_CTRLBCLR_DIR; // clear(0) for up counting

	// 7 Enable capture operation if desired in CTRLA.CAPTEN // not needed
	TC_INST->CTRLA.reg &= ~TC_CTRLA_CAPTEN0;
	TC_INST->CTRLA.reg &= ~TC_CTRLA_CAPTEN1;

	// 8 Enable inversion, if desired, in DRVCTRL.INVEN // not needed
	TC_INST->DRVCTRL.reg &= ~TC_DRVCTRL_INVEN_Msk; // disable inversion on both channels

	while(TC_INST->SYNCBUSY.reg & TC_SYNCBUSY_STATUS);

	// Enable TC
	// - set ENABLE bit in CTRLA
	TC_INST->CTRLA.reg |= TC_CTRLA_ENABLE;
	return;
}

void EE_Cntr_Init(void){
	ee_gpio_config();
	ee_gclk_config();
	ee_cntr_config();

	return;
}

unsigned int get_count(void){
	TC_INST->CTRLBSET.reg |= TC_CTRLBSET_CMD_READSYNC;
	while(TC_INST->CTRLBSET.reg & TC_CTRLBSET_CMD_READSYNC);
	return TC_INST->COUNT.reg;
}

