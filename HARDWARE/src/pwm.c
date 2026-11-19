#include "pwm.h"
#include "delay.h"

void sg90_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);

    TIM_TimeBaseStructure.TIM_Period = 2000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 840 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 50;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void sg90_set_angle(u8 angle)
{
    u8 duty;
    duty = (10.0 * angle / 9.0) + 50;
    TIM_SetCompare2(TIM2, duty);
}

/* 由 CCR2 反算当前角度：angle = (duty - 50) * 9 / 10 */
u8 sg90_get_angle(void)
{
    u16 duty = TIM_GetCapture2(TIM2);
    int deg;

    if (duty <= 50)
        return 0;
    deg = (int)(duty - 50) * 9 / 10;
    if (deg > 180)
        deg = 180;
    return (u8)deg;
}

void fan_pwm_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef gpio_struct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    gpio_struct.GPIO_Mode = GPIO_Mode_AF;      // 模式
    gpio_struct.GPIO_Pin = GPIO_Pin_8;         // 引脚
    gpio_struct.GPIO_Speed = GPIO_Speed_50MHz; // 速度
    gpio_struct.GPIO_OType = GPIO_OType_PP;    // 推挽
    gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // 无上下拉

    GPIO_Init(GPIOC, &gpio_struct);

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);

    TIM_TimeBaseStructure.TIM_Period = 10000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM3, ENABLE);
    TIM_GenerateEvent(TIM3, TIM_EventSource_Update); /* 同步预装载影子寄存器，避免首帧输出异常 */
    TIM_Cmd(TIM3, ENABLE);
}

void fan_set_speed(u32 speed)
{
    TIM_SetCompare3(TIM3, speed);
}

void fan_set_gear(u8 gear)
{
    u32 target_speed;
    u16 cur_spd;

    switch (gear)
    {
    case 0: target_speed = 0; break;
    case 1: target_speed = 3000; break;
    case 2: target_speed = 6000; break;
    case 3: target_speed = 9000; break;
    default: return;
    }

    /* 启动助推：风扇从静止(CCR=0)切到非0档时，静摩擦力较大，
       低占空比不足以启动，先满速输出一小段时间让风扇转起来，再降到目标档位 */
    cur_spd = TIM_GetCapture3(TIM3);
    if (gear > 0 && cur_spd == 0)
    {
        fan_set_speed(10000); /* 满速助推 */
        delay_ms(300);       /* 助推持续约300ms，足够风扇克服静摩擦启动 */
    }

    fan_set_speed(target_speed);
}

/* 由 CCR3 反算档位（1 档 3000、2 档 6000、3 档 9000） */
u8 fan_get_gear(void)
{
    u16 spd = TIM_GetCapture3(TIM3);

    if (spd >= 9000)
        return 3;
    if (spd >= 6000)
        return 2;
    if (spd >= 3000)
        return 1;
    return 0;
}

void led_pwm_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef gpio_struct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    gpio_struct.GPIO_Mode = GPIO_Mode_AF;                           // 模式
    gpio_struct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14; // 引脚
    gpio_struct.GPIO_Speed = GPIO_Speed_50MHz;                      // 速度
    gpio_struct.GPIO_OType = GPIO_OType_PP;                         // 推挽
    gpio_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;                       // 无上下拉

    GPIO_Init(GPIOD, &gpio_struct);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);

    TIM_TimeBaseStructure.TIM_Period = 200 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 840 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;

    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;

    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
}

void led_set_bright(u8 id, u8 bright)
{
    switch (id)
    {
    case 1:
        TIM_SetCompare1(TIM4, bright);
        break;
    case 2:
        TIM_SetCompare2(TIM4, bright);
        break;
    case 3:
        TIM_SetCompare3(TIM4, bright);
        break;
    default:
        break;
    }
}

u8 led_flag = 0;
u16 bright = 0;
void brea_light(void)
{
    if (led_flag == 0)
    {
        bright++;
    }
    if (bright >= 200)
    {
        led_flag = 1;
    }
    if (led_flag == 1)
    {
        bright--;
    }
    if (bright <= 0)
    {
        led_flag = 0;
    }
    led_set_bright(1, bright);
}

void bl_pwm_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);             // 初始化
    GPIO_SetBits(GPIOD, GPIO_Pin_4);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 10 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

    NVIC_InitStruct.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);

    TIM_Cmd(TIM5, ENABLE);
}

/* 当前亮度档位 */
volatile u8 bl_level = 0;
/* 一个PWM周期里的计数 */
volatile u8 bl_cnt = 0;

void TIM5_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
        if (++bl_cnt >= BL_MAX_LEVEL)
        {
            bl_cnt = 0;
        }
        if (bl_cnt < bl_level)
        {
            GPIO_SetBits(GPIOD, GPIO_Pin_4);
        }
        else
        {
            GPIO_ResetBits(GPIOD, GPIO_Pin_4);
        }
    }
}

u8 bl_level_next(void)
{
    bl_level += BL_STEP;
    if (bl_level > BL_MAX_LEVEL)
    {
        bl_level = 0;
    }
    return bl_level;
}
