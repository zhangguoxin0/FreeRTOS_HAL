#include "APP_FreeRTOS.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "KEY.h"
#include "semphr.h"
#include "queue.h"

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
 *                                                队列集相关
 ***********************************************************************************************************/
QueueSetHandle_t queueset_handle; // 队列集句柄

/***********************************************************************************************************
 *                                                 队列相关
 ***********************************************************************************************************/
QueueHandle_t queue_handle; // 队列句柄

/***********************************************************************************************************
 *                                                信号量相关
 ***********************************************************************************************************/
QueueHandle_t semphore_handle; // 信号量句柄

/***********************************************************************************************************
 *                                              FreeRTOS初始化
 ***********************************************************************************************************/
void App_FreeRTOS_Init(void)
{
    // 创建队列集
    queueset_handle = xQueueCreateSet(2);
    if (queueset_handle == NULL)
    {
        printf("队列集创建失败\n");
    }
    else
    {
        printf("队列集创建成功\n");
    }
    // 创建队列
    queue_handle = xQueueCreate(1, sizeof(uint8_t));
    // 创建信号量
    semphore_handle = xSemaphoreCreateBinary();
    // 添加队列和信号量到队列集
    xQueueAddToSet(queue_handle, queueset_handle);
    xQueueAddToSet(semphore_handle, queueset_handle);

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
 * @brief 通过按键状态向队列集中添加队列或信号量
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
            // 当按键1按下，将键值添加到队列集的队列中
            xQueueSend(queue_handle, &key_value, portMAX_DELAY);
        }
        else if (key_value == KEY2_PRESS)
        {
            // 当按键2按下，将释放二值信号量
            xSemaphoreGive(semphore_handle);
        }
        vTaskDelay(10);
    }
}

/**
 * @brief 获取队列集消息
 *
 * @param pvParameters
 */
void task2(void *pvParameters)
{
    QueueSetMemberHandle_t member_handle;
    uint8_t key_value;

    while (1)
    {
        // 从队列集中读取消息
        member_handle = xQueueSelectFromSet(queueset_handle, portMAX_DELAY);

        // 队列中有内容
        if (member_handle == queue_handle)
        {
            xQueueReceive(queue_handle, &key_value, portMAX_DELAY); // 获取队列中的内容
            printf("获取到的队列数据：%d\n", key_value);
        }
        // 有信号量被释放
        else if (member_handle == semphore_handle)
        {
            xSemaphoreTake(semphore_handle, portMAX_DELAY); // 获取二值信号量
            printf("获取信号量成功\n");
        }
        // 否则member_handle = NULL
    }
}
