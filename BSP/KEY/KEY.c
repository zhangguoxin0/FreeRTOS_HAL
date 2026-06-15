#include "KEY.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"

// 按键电平默认状态
static uint8_t key1_last_status = 1;
static uint8_t key1_now_status = 1;
static uint8_t key2_last_status = 1;
static uint8_t key2_now_status = 1;
static uint8_t key3_last_status = 0;
static uint8_t key3_now_status = 0;

void KEY_Init()
{
}

/**
 * @brief 按键扫描
 *
 * @return uint8_t 0：按键未被按下 x:按键x被按下
 */
uint8_t KEY_SACN(void)
{
    // 判断按键当前电平状态，更新上次电平状态
    if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
    {
        key1_last_status = key1_now_status;
        key1_now_status = 0;
    }
    else
    {
        key1_last_status = key1_now_status;
        key1_now_status = 1;
    }
    if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
    {
        key2_last_status = key2_now_status;
        key2_now_status = 0;
    }
    else
    {
        key2_last_status = key2_now_status;
        key2_now_status = 1;
    }
    if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_SET)
    {
        key3_last_status = key3_now_status;
        key3_now_status = 1;
    }
    else
    {
        key3_last_status = key3_now_status;
        key3_now_status = 0;
    }
    // 若当前电平状态为与不等于默认电平状态且上次电平状态为默认电平，则说明按键被按下
    if (key1_last_status == 1 && key1_now_status == 0)
    {
        return KEY1_PRESS;
    }
    else if (key2_last_status == 1 && key2_now_status == 0)
    {
        return KEY2_PRESS;
    }
    else if (key3_last_status == 0 && key3_now_status == 1)
    {
        return KEY3_PRESS;
    }
    // 没有按键被按下
    return 0;
}
