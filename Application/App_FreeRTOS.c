#include "APP_FreeRTOS.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "KEY.h"
#include "event_groups.h"

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
EventGroupHandle_t eventgroup_handle;
#define EVENTBIT_0 0x000001
#define EVENTBIT_1 0x000010

/***********************************************************************************************************
 *                                              FreeRTOS初始化
 ***********************************************************************************************************/
void App_FreeRTOS_Init(void)
{
    // 创建事件标志组
    eventgroup_handle = xEventGroupCreate();
    if (eventgroup_handle == NULL)
    {
        printf("事件标志组创建失败\n");
        while (1)
        {
        }
    }
    else
    {
        printf("事件标志组创建成功\n");
    }

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
 * @brief 读取按键按下值，根据不同键值将事件标志组相应事件位置1，模拟事件发生
 *
 * @param pvParameters
 */
void task1(void *pvParameters)
{
    uint8_t key_value;

    while (1)
    {
        key_value = KEY_SACN();
        if (key_value == KEY1_PRESS)
        {
            xEventGroupSetBits(eventgroup_handle, EVENTBIT_0);
        }
        else if (key_value == KEY2_PRESS)
        {
            xEventGroupSetBits(eventgroup_handle, EVENTBIT_1);
        }
        vTaskDelay(10);
    }
}

/**
 * @brief 同时等待事件标志组中的多个事件位，当这些事件位都置1的话就执行相应的处理
 *
 * @param pvParameters
 */
void task2(void *pvParameters)
{
    EventBits_t event_bit;
    while (1)
    {
        event_bit = xEventGroupWaitBits(eventgroup_handle, EVENTBIT_0 | EVENTBIT_1, pdTRUE, pdTRUE, portMAX_DELAY);
        printf("等待到的事件标志位为：%.6x\n", event_bit);
    }
}
