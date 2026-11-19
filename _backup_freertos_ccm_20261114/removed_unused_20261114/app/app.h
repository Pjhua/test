#ifndef __APP_H
#define __APP_H

/* 三个子界面，由主界面点击对应图标进入，点击左上角“返回”按钮回到主界面 */
void app_control(void); /* 控制界面：蜂鸣器、LED、风扇、继电器 */
void app_look(void);    /* 查看界面：温湿度、MPU6050数据 */
void app_clock(void);   /* 时钟界面：文字时间 + 指针时钟 */

void app_control_new(void);
void app_album(void);
void app_ebook(void);
void app_music(void);

#endif
