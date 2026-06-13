#include "APP_FreeRTOS.h"
#include "usart.h"
#include "LED.h"
#include "KEY.h"

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
// TASK2任务配置
void task2(void *pvParameters);
#define TASK2_STACK_SIZE 128
#define TASK2_PRIO 3
TaskHandle_t task2_handler;
// TASK2任务配置
void task3(void *pvParameters);
#define TASK3_STACK_SIZE 128
#define TASK3_PRIO 4
TaskHandle_t task3_handler;

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
    xTaskCreate(task2, "task2", TASK2_STACK_SIZE, NULL, TASK2_PRIO, &task2_handler);
    xTaskCreate(task3, "task3", TASK3_STACK_SIZE, NULL, TASK3_PRIO, &task3_handler);
    vTaskDelete(start_task_handler);
    taskEXIT_CRITICAL(); // 退出临界区
}

/**
 * @brief 实现LED1每1s翻转一次
 *
 * @param pvParameters
 */
void task1(void *pvParameters)
{
    uint8_t task1_RunNum;
    while (1)
    {
        printf("task1运行%d次\n", ++task1_RunNum);
        LED1_Toggle();
        vTaskDelay(500);
    }
}

/**
 * @brief 实现LED2每500ms翻转一次
 *
 * @param pvParameters
 */
void task2(void *pvParameters)
{
    uint8_t task2_RunNum;
    while (1)
    {
        printf("task2运行%d次\n", ++task2_RunNum);
        LED2_Toggle();
        vTaskDelay(500);
    }
}

/**
 * @brief 判断按键按下删除task1
 *
 * @param pvParameters
 */
void task3(void *pvParameters)
{
    uint8_t key_status = 0;

    while (1)
    {
        key_status = KEY_SACN();
        if (key_status == KEY1_PRESS)
        {
            vTaskSuspend(task1_handler);
            printf("task1被挂起\n");
        }
        else if (key_status == KEY2_PRESS)
        {
            vTaskResume(task1_handler);
            printf("在任务中恢复task1\n");
        }
        vTaskDelay(10);
    }
}
