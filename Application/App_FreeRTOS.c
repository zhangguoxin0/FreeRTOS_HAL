#include "APP_FreeRTOS.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "KEY.h"
#include "semphr.h"

/***********************************************************************************************************
 *                                                任务配置
 ***********************************************************************************************************/
// 启动任务配置
void start_task(void *pvParameters);
#define START_TASK_STACK_SIZE 128
#define STACK_TASK_PRIO 1
TaskHandle_t start_task_handler;
// low_task任务配置
void low_task(void *pvParameters);
#define LOW_TASK_STACK_SIZE 128
#define LOW_TASK_PRIO 2
TaskHandle_t low_task_handler;
// middle_task任务配置
void middle_task(void *pvParameters);
#define MIDDLE_TASK_STACK_SIZE 128
#define MIDDLE_TASK_PRIO 3
TaskHandle_t middle_task_handler;
// high_task任务配置
void high_task(void *pvParameters);
#define HIGH_TASK_STACK_SIZE 128
#define HIGH_TASK_PRIO 4
TaskHandle_t high_task_handler;

/***********************************************************************************************************
 *                                                信号量相关
 ***********************************************************************************************************/
QueueHandle_t semphore_handle; // 信号量句柄

/***********************************************************************************************************
 *                                              FreeRTOS初始化
 ***********************************************************************************************************/
void App_FreeRTOS_Init(void)
{
    // 创建计数型信号量
    semphore_handle = xSemaphoreCreateBinary();
    if (semphore_handle == NULL)
    {
        printf("信号量创建失败\n");
        while (1)
        {
        }
    }
    else
    {
        printf("信号量创建成功\n");
        // 释放信号量
        xSemaphoreGive(semphore_handle);
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
    xTaskCreate(low_task, "low_task", LOW_TASK_STACK_SIZE, NULL, LOW_TASK_PRIO, &low_task_handler);
    xTaskCreate(middle_task, "middle_task", MIDDLE_TASK_STACK_SIZE, NULL, MIDDLE_TASK_PRIO, &middle_task_handler);
    xTaskCreate(high_task, "high_task", HIGH_TASK_STACK_SIZE, NULL, HIGH_TASK_PRIO, &high_task_handler);
    vTaskDelete(start_task_handler);
    taskEXIT_CRITICAL(); // 退出临界区
}

/**
 * @brief 低优先级任务：获取信号量，占用信号量时间更久
 *
 * @param pvParameters
 */
void low_task(void *pvParameters)
{
    while (1)
    {
        printf("低优先级任务获取信号量\n");
        xSemaphoreTake(semphore_handle, portMAX_DELAY);
        printf("低优先级任务正在执行\n");
        HAL_Delay(3000);
        printf("低优先级任务释放信号量\n");
        xSemaphoreGive(semphore_handle);
        vTaskDelay(10);
    }
}

/**
 * @brief 中优先级任务：抢占低优先级任务
 *
 * @param pvParameters
 */
void middle_task(void *pvParameters)
{
    while (1)
    {
        printf("中优先级任务正在执行\n");
        vTaskDelay(1000);
    }
}

/**
 * @brief 高优先级任务：获取信号量，占用信号量时间更短
 *
 * @param pvParameters
 */
void high_task(void *pvParameters)
{
    while (1)
    {
        printf("高优先级任务获取信号量\n");
        xSemaphoreTake(semphore_handle, portMAX_DELAY);
        printf("高优先级任务正在执行\n");
        HAL_Delay(1000);
        printf("高优先级任务释放信号量\n");
        xSemaphoreGive(semphore_handle);
        vTaskDelay(10);
    }
}

/* 
串口打印结果
--------------------------------------
信号量创建成功
高优先级任务获取信号量
高优先级任务正在执行
高优先级任务释放信号量
中优先级任务正在执行
低优先级任务获取信号量
低优先级任务正在执行
高优先级任务获取信号量  <-此处高优先级任务获取信号量失败(被阻塞),后续直接中优先级任务开始执行
中优先级任务正在执行
中优先级任务正在执行
中优先级任务正在执行
低优先级任务释放信号量  <-此处低优先级任务释放信号量
高优先级任务正在执行    <-高优先级任务抢占低优先级任务
高优先级任务释放信号量
*/
