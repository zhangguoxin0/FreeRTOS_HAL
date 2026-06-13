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
StackType_t start_task_stack[START_TASK_STACK_SIZE];
StaticTask_t start_task_tcb;
// TASK1任务配置
void task1(void *pvParameters);
#define TASK1_STACK_SIZE 128
#define TASK1_PRIO 2
TaskHandle_t task1_handler;
StackType_t task1_stack[TASK1_STACK_SIZE];
StaticTask_t task1_tcb;
// TASK2任务配置
void task2(void *pvParameters);
#define TASK2_STACK_SIZE 128
#define TASK2_PRIO 3
TaskHandle_t task2_handler;
StackType_t task2_stack[TASK2_STACK_SIZE];
StaticTask_t task2_tcb;
// TASK2任务配置
void task3(void *pvParameters);
#define TASK3_STACK_SIZE 128
#define TASK3_PRIO 4
TaskHandle_t task3_handler;
StackType_t task3_stack[TASK3_STACK_SIZE];
StaticTask_t task3_tcb;

/********************************************** 静态创建任务接口函数 **********************************************/

StaticTask_t idle_task_tcb;
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
/**
 * @brief 空闲任务内存分配
 *
 * @param ppxIdleTaskTCBBuffer
 * @param ppxIdleTaskStackBuffer
 * @param pulIdleTaskStackSize
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer = &idle_task_tcb;
    *ppxIdleTaskStackBuffer = idle_task_stack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/********************************************** FreeRTOS初始化 **********************************************/

void App_FreeRTOS_Init(void)
{
    start_task_handler = xTaskCreateStatic(start_task, "start_task", START_TASK_STACK_SIZE, NULL, STACK_TASK_PRIO, start_task_stack, &start_task_tcb);

    vTaskStartScheduler();
}

/********************************************** 任务函数实体 **********************************************/

void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); // 进入临界区
    task1_handler = xTaskCreateStatic(task1, "task1", TASK1_STACK_SIZE, NULL, TASK1_PRIO, task1_stack, &task1_tcb);
    task2_handler = xTaskCreateStatic(task2, "task2", TASK2_STACK_SIZE, NULL, TASK2_PRIO, task2_stack, &task2_tcb);
    task3_handler = xTaskCreateStatic(task3, "task3", TASK3_STACK_SIZE, NULL, TASK3_PRIO, task3_stack, &task3_tcb);
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
    while (1)
    {
        printf("task1运行中...\n");
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
    while (1)
    {
        printf("task2运行中...\n");
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
        key_status = KEY1_SACN();
        if (key_status == 1)
        {
            if (task1_handler != NULL)
            {
                printf("task1被删除!\n");
                vTaskDelete(task1_handler);
                task1_handler = NULL;
            }
        }
        vTaskDelay(10);
    }
}
