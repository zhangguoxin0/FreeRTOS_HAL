#include "APP_FreeRTOS.h"
#include "usart.h"
#include "tim.h"

/********************************************** 任务配置 **********************************************/

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

/********************************************** FreeRTOS初始化 **********************************************/

void App_FreeRTOS_Init(void)
{
    xTaskCreate(start_task, "start_task", START_TASK_STACK_SIZE, NULL, STACK_TASK_PRIO, &start_task_handler);

    vTaskStartScheduler();
}

/********************************************** 任务函数实体 **********************************************/

void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); // 进入临界区
    xTaskCreate(task1, "task1", TASK1_STACK_SIZE, NULL, TASK1_PRIO, &task1_handler);
    vTaskDelete(start_task_handler);
    taskEXIT_CRITICAL(); // 退出临界区
}


void task1(void *pvParameters)
{
    uint8_t task1_RunNum = 0;
    while (1)
    {
        if(++task1_RunNum == 5)
        {
            printf("关中断\n");
            portDISABLE_INTERRUPTS();
            HAL_Delay(5000);
            printf("开中断\n");
            portENABLE_INTERRUPTS();
        }
        vTaskDelay(1000);
    }
}

