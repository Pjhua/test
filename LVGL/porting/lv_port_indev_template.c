/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

#include "lv_port_indev_template.h"
#include "CST816S.h"



static void touchpad_init(void);
static void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data);



lv_indev_t *indev_touchpad;



void lv_port_indev_init(void)
{
    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev_touchpad, touchpad_read);
}

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    /*Your code comes here*/
    CST816S_Init();
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    static int32_t last_x = 0;
    static int32_t last_y = 0;

    u16 x_pos, y_pos;
    u8 ret = CST816S_GetPoint(&x_pos, &y_pos);

    if(ret == 0) {
        /* ret==0: 有手指按下, 更新坐标并上报 PRESSED */
        last_x = x_pos;
        last_y = y_pos;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else {
        /* ret!=0: 无手指, 上报 RELEASED, 保留最后坐标 */
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->point.x = last_x;
    data->point.y = last_y;
}



#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
