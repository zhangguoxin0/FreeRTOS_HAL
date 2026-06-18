#include "APP_FreeRTOS.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
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

/***********************************************************************************************************
 *                                            软件定时器任务配置
 ***********************************************************************************************************/
// 单次定时器
TimerHandle_t timeronce_handle;                 // 单次定时器任务句柄
#define TIMERONCE_ID (void *)1                  // 单次定时器ID
void timeronce_callback(TimerHandle_t pxTimer); // 单次定时器回调函数
// 周期定时器
TimerHandle_t timercycle_handle;                 // 周期定时器任务句柄
#define TIMERCYCLE_ID (void *)2                  // 周期定时器ID
void timercycle_callback(TimerHandle_t pxTimer); // 周期定时器回调函数

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
    timeronce_handle = xTimerCreate("timeronce", 1000, pdFALSE, TIMERONCE_ID, timeronce_callback);
    timercycle_handle = xTimerCreate("timercycle", 1000, pdTRUE, TIMERCYCLE_ID, timercycle_callback);
    xTaskCreate(task1, "task1", TASK1_STACK_SIZE, NULL, TASK1_PRIO, &task1_handler);
    vTaskDelete(start_task_handler);
    taskEXIT_CRITICAL(); // 退出临界区
}

/**
 * @brief 通过按键对软件定时器进行操作
 * 
 * @param pvParameters 
 */
void task1(void *pvParameters)
{
    uint32_t key_value;
    while (1)
    {
        key_value = KEY_SACN();
        // 按键1按下，开启定时器
        if(key_value == KEY1_PRESS)
        {
            xTimerStart(timeronce_handle, portMAX_DELAY);
            xTimerStart(timercycle_handle, portMAX_DELAY);
        }
        // 按键2按下，关闭定时器
        else if(key_value == KEY2_PRESS)
        {
            xTimerStop(timeronce_handle, portMAX_DELAY);
            xTimerStop(timercycle_handle, portMAX_DELAY);
        }
        vTaskDelay(10);
    }
}

/***********************************************************************************************************
 *                                                回调函数
 ***********************************************************************************************************/
/**
 * @brief 单次定时器回调函数
 *
 * @param pxTimer
 */
void timeronce_callback(TimerHandle_t pxTimer)
{
    LED1_Toggle();
}

/**
 * @brief 周期定时器回调函数
 *
 * @param pxTimer
 */
void timercycle_callback(TimerHandle_t pxTimer)
{
    LED2_Toggle();
}
