#include "freertos_app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "key.h"
#include "delay.h"
#include "event_groups.h"
#include "lcd.h"
#include "esp8266.h"
#include "sht3x.h"
#include "mpu6050.h"
#include "timers.h"

TaskHandle_t startHandle;
TaskHandle_t measureHandle;
TaskHandle_t lcdHandle;
TaskHandle_t wifiHandle;
TaskHandle_t lowHandle;
TaskHandle_t middleHandle;
TaskHandle_t highHandle;

EventGroupHandle_t eventgroup;
QueueHandle_t queue;

#define BIT(n) 1 << n
#define KEY_EVENT BIT(0)
#define MEASURE_EVENT BIT(1)
#define LCD_EVENT BIT(2)
#define WIFI_EVENT BIT(3)

void freertos_init(void)
{
    xTaskCreate(start_task, "stast task", 128, NULL, 1, &startHandle);
    vTaskStartScheduler();
}

void start_task(void *acg)
{
    printf("%s starting...\r\n", __func__);

    taskENTER_CRITICAL(); // 进入临界区

    queue = xQueueCreate(5, sizeof(buf_t));
    if (queue == NULL)
    {
        printf("queue create error\r\n");
    }

    eventgroup = xEventGroupCreate(); // 创建事件标志组
    if (eventgroup == NULL)
    {
        printf("eventgroup create error\r\n");
    }

    xTaskCreate(low_task, "low task", 128, NULL, 3, &lowHandle);
    xTaskCreate(middle_task, "middle task", 128, NULL, 5, &middleHandle);
    xTaskCreate(high_task, "high task", 128, NULL, 20, &highHandle);
    xTaskCreate(measure_task, "measure task", 512, NULL, 10, &measureHandle);
    xTaskCreate(lcd_task, "lcd task", 1024, NULL, 10, &lcdHandle);
    xTaskCreate(wifi_task, "wifi task", 256, NULL, 1, &wifiHandle);

    taskEXIT_CRITICAL(); // 退出临界区

    vTaskDelete(NULL);
}

void measure_task(void *acg)
{
    BaseType_t ret;
    buf_t data = {0};
    Sht3x_TypeDef HT;

    while (1)
    {
        Sht3x_Read_TemperatureHumidity(&HT);
        data.hum = HT.humidity;
        data.temp = HT.temperature;
        Mpu6050_GetGyroscope(&data.gx, &data.gy, &data.gz);
        ret = xQueueSend(queue, &data, portMAX_DELAY);
        if (ret == pdTRUE)
        {
            xEventGroupSetBits(eventgroup, LCD_EVENT);
        }
        delay_ms(1000);
    }
}

typedef struct
{
    char gx[10];
    char gy[10];
    char gz[10];
    char humidity[10];
    char temperature[10];
} data_t;

void lcd_task(void *acg)
{
    BaseType_t ret;
    EventBits_t event_bits;
    buf_t buf;
    data_t data;

    LCD_ShowString(20, 0, "temp:", BLUE, WHITE, 32, 0);
    LCD_ShowString(20, 32, "hum:", BLUE, WHITE, 32, 0);

    LCD_ShowString(20, 64, "gx:", BLUE, WHITE, 32, 0);
    LCD_ShowString(20, 96, "gy:", BLUE, WHITE, 32, 0);
    LCD_ShowString(20, 128, "gz:", BLUE, WHITE, 32, 0);

    LCD_ShowString(20, 160, "wifi:", BLUE, WHITE, 32, 0);

    while (1)
    {
        event_bits = xEventGroupWaitBits(eventgroup, LCD_EVENT | WIFI_EVENT, pdTRUE, pdFALSE, portMAX_DELAY);
        if (event_bits & LCD_EVENT)
        {
            ret = xQueueReceive(queue, &buf, portMAX_DELAY);
            if (ret == pdTRUE)
            {
                sprintf(data.humidity, "%.1f", buf.hum);
                sprintf(data.temperature, "%.1f", buf.temp);
                sprintf(data.gx, "%d", buf.gx);
                sprintf(data.gy, "%d", buf.gy);
                sprintf(data.gz, "%d", buf.gz);

                LCD_Fill(100, 0, 200, 160, WHITE);

                LCD_ShowString(100, 0, (u8 *)data.temperature, BLUE, WHITE, 32, 0);
                LCD_ShowString(100, 32, (u8 *)data.humidity, BLUE, WHITE, 32, 0);
                LCD_ShowString(100, 64, (u8 *)data.gx, BLUE, WHITE, 32, 0);
                LCD_ShowString(100, 96, (u8 *)data.gy, BLUE, WHITE, 32, 0);
                LCD_ShowString(100, 128, (u8 *)data.gz, BLUE, WHITE, 32, 0);
            }
        }
        if (event_bits & WIFI_EVENT)
        {
            LCD_ShowString(100, 160, (u8 *)"wifi ok", BLUE, WHITE, 32, 0);
        }
    }
}

void wifi_task(void *acg)
{
    if (esp8266_connect_wifi() == 0)
    {
        xEventGroupSetBits(eventgroup, WIFI_EVENT);
    }
    while (1)
    {

        delay_ms(1000);
    }
}

void low_task(void *arg)
{
    u32 ret;
    while (1)
    {
        ret = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        if (ret != pdFAIL)
        {
            printf("low task 接到任务通知,通知值为:%d\r\n", ret);
        }
        printf("low task 正在运行...\r\n");

        delay_ms(10);
    }
}

void middle_task(void *arg)
{
    BaseType_t ret;
    u32 get_value = 0;
    while (1)
    {
        ret = xTaskNotifyWait(0, 0, &get_value, portMAX_DELAY);
        if (ret == pdTRUE)
        {
            printf("middle task 接到任务通知,通知值为:%d\r\n", get_value);
        }
        printf("middle task 正在运行...\r\n");
        delay_ms(10);
    }
}

void high_task(void *arg)
{
    u8 key;
    BaseType_t ret;
    while (1)
    {
        key = key_scan();
        if (key == KEY1_PRE)
        {
            printf("key1 发送任务通知\r\n");
            xTaskNotifyGive(lowHandle);
        }
        if (key == KEY2_PRE)
        {
            printf("key2 发送任务通知\r\n");
            xTaskNotify(middleHandle, 100, eSetValueWithoutOverwrite);
        }
        if (key == KEY3_PRE)
        {
        }
        delay_ms(10);
    }
}
