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
        if (key_value != KEY_NOPRESS)
        {
            // 将键值覆盖写入到task2的任务通知值上
            printf("键值：%d 写入 task2 任务通知值成功！\n",key_value);
            xTaskNotify(task2_handler, key_value, eSetValueWithOverwrite);
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
    uint32_t key_value;
    while (1)
    {
        res = xTaskNotifyWait(0, 0, &key_value, portMAX_DELAY);
        if (res == pdTRUE)
        {
            printf("接收任务通知成功，任务通知模拟消息队列获取消息！\n");
            printf("获取到的键值：%d\n", key_value);
            if(key_value == KEY1_PRESS)
            {
                LED1_Toggle();
            }
            else if(key_value == KEY2_PRESS)
            {
                LED2_ON();
            }
            else if(key_value == KEY3_PRESS)
            {
                LED2_OFF();
            }
        }

        // 通过延时1s,在1s到来前多次按下不同按键验证覆盖写入功能
        vTaskDelay(1000);
    }
}
