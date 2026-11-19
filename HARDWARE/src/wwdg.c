#include "wwdg.h"

void wwdg_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	WWDG_SetPrescaler(WWDG_Prescaler_4);
	WWDG_SetWindowValue(95);
	WWDG_Enable(127);
}
