#include "key.h"

void key_init(void)
{
    GPIO_InitTypeDef gpio_struct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    gpio_struct.GPIO_Mode = GPIO_Mode_IN;
    gpio_struct.GPIO_Pin = GPIO_Pin_0;
    gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_struct);

    gpio_struct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &gpio_struct);
}

u8 key_scan(void)
{
    static u8 key_flag = 1;
    if ((KEY1 == 1 || KEY2 == 0 || KEY3 == 0) && key_flag == 1)
    {
        key_flag = 0;
        delay_ms(10);
        if (KEY1 == 1)
            return KEY1_PRE;
        if (KEY2 == 0)
            return KEY2_PRE;
        if (KEY3 == 0)
            return KEY3_PRE;
    }
    else if (KEY1 == 0 && KEY2 == 1 && KEY3 == 1)
    {
        key_flag = 1;
    }
    return 0;
}

/*********************************************************************************************************
 * 函 数 名 : Scanf_Key
 * 功能说明 : 按键扫描
 * 形    参 : 无
 * 返 回 值 : 所有按键的状态
 * 备    注 : PA0（SWITCH_KEY）
 *********************************************************************************************************/
u8 Scanf_Key(void)
{
    static unsigned char state = KEY_FREE; // 初始空闲状态
    static unsigned char cnt = 0;
    static unsigned char key = 0;

    switch (state)
    {
    case KEY_FREE: // 检测是否有按键按下，有则进入消抖状态
        if (KEY1 == 1 || KEY2 == 0 || KEY3 == 0)
            state = KEY_SHAKE;
        break;
    case KEY_SHAKE:
        if (++cnt > 5) // 消抖50ms，如果50ms后按键还是按下状态，认为真的按下，否则认为抖动
        {
            cnt = 0;
            if (KEY1 == 1 || KEY2 == 0 || KEY3 == 0)
            {
                key = ((!!KEY1) << 0) | ((!KEY2) << 1) | ((!KEY3) << 2); // 记录按键当前值
                state = KEY_PRESS;                                       // 进入下一个状态
            }
            else
                state = KEY_FREE; // 回到检测状态
        }
        break;
    case KEY_PRESS: // 如果按下的时间超过1s，那么认为是长按，返回结果，并设置下一个状态为等待松手态
        if (KEY1 == 1 || KEY2 == 0 || KEY3 == 0)
        {
            if (++cnt > 100)
            {
                state = KEY_LOOSEN;
                return (key | KEY_LONG_PRESS); // 最高位标记长按短按
            }
        }
        else
        {
            state = KEY_LOOSEN;             // 设置下一个状态为等待松手态
            return (key | KEY_SHORT_PRESS); // 如果在1s的时间内松手，那么认为短按并返回结果，
        }
        break;
    case KEY_LOOSEN: // 所有按键松开，则认为本次按下结束，回到最初的状态
        if (KEY1 == false && KEY2 == true && KEY3 == true)
        {
            state = KEY_FREE;
            cnt = 0;
            key = 0;
        }
        break;
    }
    return 0;
}
