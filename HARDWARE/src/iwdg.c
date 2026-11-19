#include "iwdg.h"

void iwdg_init(void)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    IWDG_SetPrescaler(IWDG_Prescaler_32); // 分频

    IWDG_SetReload(1000); // 时间

    IWDG_ReloadCounter(); // 重新加载计数值，相当于喂狗

    IWDG_Enable();
}
