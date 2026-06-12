#include "KEY.h"

void KEY_Init()
{
}

/**
 * @brief 按键扫描
 * 
 * @return uint8_t 1：按键被按下 0：按键未被按下
 */
uint8_t KEY1_SACN(void)
{
    if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
    {
        return 1;
    }

    return 0;
}
