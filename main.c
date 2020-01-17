/**
 * @{
 * @brief	Counting external triggers using timer/counter
 *
 *
 * @}
 */

#include <stdio.h>
#include "xtimer.h"

#include "external_event_counter.h"

int main(void){
	EE_Cntr_Init(); // external event counter

	while(1){
		xtimer_usleep(100000);

		printf("\nTC COUNT: %d\n", get_count());
		printf("\n%lx\n", GCLK->GENCTRL[GCLK_N].reg); // GENCTRL
		printf("\n%lx\n", GCLK->PCHCTRL[PCH_M].reg);  // PCHCTRL
	}

    return 0;
}
