#include "APP_FreeRTOS.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "KEY.h"

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
    vTaskDelete(start_task_handler);
    taskEXIT_CRITICAL(); // 退出临界区
}

/**
 * @brief 申请和释放内存
 *
 * @param pvParameters
 */
void task1(void *pvParameters)
{
    uint8_t key_value;    // 键值
    uint32_t *buf = NULL; // 申请到的内存首地址

    while (1)
    {
        key_value = KEY_SACN();
        if (key_value == KEY1_PRESS)
        {
            buf = pvPortMalloc(30); // 申请内存
            if (buf == NULL)
            {
                printf("申请内存失败\n");
            }
            else
            {
                printf("申请内存成功\n");
            }
        }
        if (key_value == KEY2_PRESS)
        {
            if (buf != NULL)
            {
                vPortFree(buf); // 释放内存
                printf("释放内存\n");
            }
        }
        if (key_value == KEY3_PRESS)
        {
            printf("剩余的空闲内存大小为：%d\n", xPortGetFreeHeapSize());
        }
        vTaskDelay(10);
    }
}
