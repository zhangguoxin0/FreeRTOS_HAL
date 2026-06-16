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
// TASK1任务配置
void task1(void *pvParameters);
#define TASK1_STACK_SIZE 128
#define TASK1_PRIO 2
TaskHandle_t task1_handler;
// TASK2任务配置
void task2(void *pvParameters);
#define TASK2_STACK_SIZE 128
#define TASK2_PRIO 3
TaskHandle_t task2_handler;

/***********************************************************************************************************
 *                                              二值信号量相关
 ***********************************************************************************************************/
QueueHandle_t semphore_handle; // 二值信号量句柄

/***********************************************************************************************************
 *                                              FreeRTOS初始化
 ***********************************************************************************************************/
void App_FreeRTOS_Init(void)
{
    // 创建二值信号量
    semphore_handle = xSemaphoreCreateBinary();
    if (semphore_handle == NULL)
    {
        printf("二值信号量创建失败\n");
        while (1)
        {
        }
    }
    else
    {
        printf("二值信号量创建成功\n");
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
 * @brief 当按键1按下时释放二值信号量
 *
 * @param pvParameters
 */
void task1(void *pvParameters)
{
    uint8_t key_value; // 键值
    BaseType_t res;

    while (1)
    {
        key_value = KEY_SACN();
        if (key_value == KEY1_PRESS)
        {
            if (semphore_handle != NULL)
            {
                res = xSemaphoreGive(semphore_handle);
                if (res == pdPASS)
                {
                    printf("二值信号量释放成功\n");
                }
                else
                {
                    printf("二值信号量释放失败\n");
                }
            }
        }
        vTaskDelay(10);
    }
}

/**
 * @brief 获取二值信号量，当获取成功时打印信息
 *
 * @param pvParameters
 */
void task2(void *pvParameters)
{
    uint8_t i;
    BaseType_t res;

    while (1)
    {
        // 获取二值信号量
        res = xSemaphoreTake(semphore_handle, 1000);
        // 根据获取情况打印对应信息
        if (res == pdTRUE)
        {
            printf("二值信号量获取成功%d次\n", ++i);
        }
        else
        {
            printf("二值信号量获取失败\n");
        }
    }
}
