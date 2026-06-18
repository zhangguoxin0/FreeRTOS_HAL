#include "APP_FreeRTOS.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "KEY.h"
#include "LED.h"

/***********************************************************************************************************
 *                                                任务配置
 ***********************************************************************************************************/
// 启动任务配置
void start_task(void *pvParameters);
#define START_TASK_STACK_SIZE 128
#define STACK_TASK_PRIO 1
TaskHandle_t start_task_handler;
// TASK1任务配置
void task1(void *pvParameters);
#define TASK1_STACK_SIZE 128
#define TASK1_PRIO 2
TaskHandle_t task1_handler;
// TASK2任务配置
void task2(void *pvParameters);
#define TASK2_STACK_SIZE 128
#define TASK2_PRIO 2
TaskHandle_t task2_handler;

/***********************************************************************************************************
 *                                              事件标志组相关
 ***********************************************************************************************************/
#define EVENTBIT_0 (1 << 0)
#define EVENTBIT_1 (1 << 1)
#define EVENTBIT_ALL 0xFFFFFFFF

/***********************************************************************************************************
 *                                              FreeRTOS初始化
 ***********************************************************************************************************/
void App_FreeRTOS_Init(void)
{
    // 创建任务
    xTaskCreate(start_task, "start_task", START_TASK_STACK_SIZE, NULL, STACK_TASK_PRIO, &start_task_handler);

    // 启动任务
    vTaskStartScheduler();
}

/***********************************************************************************************************
 *                                               任务函数实体
 ***********************************************************************************************************/
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); // 进入临界区
    xTaskCreate(task1, "task1", TASK1_STACK_SIZE, NULL, TASK1_PRIO, &task1_handler);
    xTaskCreate(task2, "task2", TASK2_STACK_SIZE, NULL, TASK2_PRIO, &task2_handler);
    vTaskDelete(start_task_handler);
    taskEXIT_CRITICAL(); // 退出临界区
}

/**
 * @brief 发送任务通知值
 *
 * @param pvParameters
 */
void task1(void *pvParameters)
{
    uint32_t key_value;
    while (1)
    {
        key_value = KEY_SACN();
        // 按键按下发送任务通知
        if (key_value == KEY1_PRESS)
        {
            printf("按键1按下,将任务通知值的bit0位置1\n");
            xTaskNotify(task2_handler, EVENTBIT_0, eSetBits);
        }
        else if (key_value == KEY2_PRESS)
        {
            printf("按键2按下,将任务通知值的bit1位置1\n");
            xTaskNotify(task2_handler, EVENTBIT_1, eSetBits);
        }
        vTaskDelay(10);
    }
}

/**
 * @brief 接收任务通知值
 *
 * @param pvParameters
 */
void task2(void *pvParameters)
{
    BaseType_t res;
    uint32_t event_bit;

    while (1)
    {
        res = xTaskNotifyWait(EVENTBIT_ALL, EVENTBIT_ALL, &event_bit, portMAX_DELAY);
        if (res == pdTRUE)
        {
            if (event_bit & EVENTBIT_0)
            {
                LED1_Toggle();
            }
            if (event_bit & EVENTBIT_1)
            {
                LED2_Toggle();
            }
        }
    }
}
