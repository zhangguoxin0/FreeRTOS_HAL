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
 *                                                信号量相关
 ***********************************************************************************************************/
QueueHandle_t count_semphore_handle; // 信号量句柄

/***********************************************************************************************************
 *                                              FreeRTOS初始化
 ***********************************************************************************************************/
void App_FreeRTOS_Init(void)
{
    // 创建计数型信号量
    count_semphore_handle = xSemaphoreCreateCounting(100, 0); // 参数：(最大值, 初始值)
    if (count_semphore_handle == NULL)
    {
        printf("计数型信号量创建失败\n");
        while (1)
        {
        }
    }
    else
    {
        printf("计数型信号量创建成功\n");
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
 * @brief 当按键1按下时释放计数型信号量
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
            if (count_semphore_handle != NULL)
            {
                res = xSemaphoreGive(count_semphore_handle);
                if (res == pdPASS)
                {
                    printf("计数型信号量释放成功\n");
                }
                else
                {
                    printf("计数型信号量释放失败\n");
                }
            }
        }
        vTaskDelay(10);
    }
}

/**
 * @brief 每1s获取计数型信号量，当获取成功时打印信号量计数值
 *
 * @param pvParameters
 */
void task2(void *pvParameters)
{
    BaseType_t res;

    while (1)
    {
        // 获取计数型信号量
        res = xSemaphoreTake(count_semphore_handle, portMAX_DELAY);
        // 根据获取情况打印对应信息
        if (res == pdTRUE)
        {
            printf("计数型信号量获取成功,信号量计数值为%d\n",uxSemaphoreGetCount(count_semphore_handle));
        }
        else
        {
            printf("计数型信号量获取失败\n");
        }
        vTaskDelay(1000);
    }
}
