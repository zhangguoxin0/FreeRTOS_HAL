#include "KEY.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"

static uint8_t key1_last_status = 1;
static uint8_t key1_now_status = 1;
static uint8_t key2_last_status = 1;
static uint8_t key2_now_status = 1;

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

    if(key1_last_status == 1 && key1_now_status == 0)
    {
        return KEY1_PRESS;
    }
    else if (key2_last_status == 1 && key2_now_status == 0)
    {
        return KEY2_PRESS;
    }
    return 0;
}

extern TaskHandle_t task1_handler;
/**
 * @brief 外部中断中断服务程序
 *
 * @param GPIO_Pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    BaseType_t xYieldRequired;

    if (GPIO_Pin == KEY3_Pin)
    {
        // 按键3被按下
        xYieldRequired = xTaskResumeFromISR(task1_handler);
        // printf("在中断中恢复task1\n");
    }

    if (xYieldRequired == pdTRUE)
    {
        // task1从挂起状态恢复需要执行任务切换
        portYIELD_FROM_ISR(xYieldRequired);
    }
}
