#include "APP_FreeRTOS.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
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
// TASK2任务配置
void task2(void *pvParameters);
#define TASK2_STACK_SIZE 128
#define TASK2_PRIO 2
TaskHandle_t task2_handler;
// TASK3任务配置
void task3(void *pvParameters);
#define TASK3_STACK_SIZE 128
#define TASK3_PRIO 2
TaskHandle_t task3_handler;

/***********************************************************************************************************
 *                                               消息队列相关
 ***********************************************************************************************************/
QueueHandle_t key_queue;                      // 小数据消息队列
QueueHandle_t big_date_queue;                 // 大数据消息队列
char buff[100] = "这是一段文本,123,abc,!!!!"; // 测试内容

/***********************************************************************************************************
 *                                              FreeRTOS初始化
 ***********************************************************************************************************/
void App_FreeRTOS_Init(void)
{
    // 创建消息队列
    key_queue = xQueueCreate(2, sizeof(uint8_t));
    if (key_queue == NULL)
    {
        printf("小数据消息队列创建失败!\n");
        while (1)
        {
        }
    }
    big_date_queue = xQueueCreate(1, sizeof(char *));
    if (big_date_queue == NULL)
    {
        printf("大数据消息队列创建失败!\n");
        while (1)
        {
        }
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
    xTaskCreate(task3, "task3", TASK3_STACK_SIZE, NULL, TASK3_PRIO, &task3_handler);
    vTaskDelete(start_task_handler);
    taskEXIT_CRITICAL(); // 退出临界区
}

/**
 * @brief 数据入队
 *
 * @param pvParameters
 */
void task1(void *pvParameters)
{
    uint8_t key_value; // 键值
    char *buf;
    BaseType_t res;

    buf = buff; // buf的值为buff的首地址

    while (1)
    {
        key_value = KEY_SACN();
        if (key_value == KEY1_PRESS || key_value == KEY2_PRESS)
        {
            // 当按键1或按键2按下，将键值发送到小数据队列中，若消息队列没有空闲位置则一直等待
            res = xQueueSend(key_queue, &key_value, portMAX_DELAY);
            if (res != pdTRUE)
            {
                printf("小数据队列写入键值失败!\n");
                while (1)
                {
                }
            }
        }
        else if (key_value == KEY3_PRESS)
        {
            // 当按键3按下，将 测试内容 发送到大数据队列中，若消息队列没有空闲则一直等待
            res = xQueueSend(big_date_queue, &buf, portMAX_DELAY);
            if (res != pdTRUE)
            {
                printf("大数据队列写入测试内容失败!\n");
                while (1)
                {
                }
            }
        }

        vTaskDelay(10);
    }
}

/**
 * @brief 小数据出队
 *
 * @param pvParameters
 */
void task2(void *pvParameters)
{
    uint8_t key_value;
    BaseType_t res;

    while (1)
    {
        res = xQueueReceive(key_queue, &key_value, portMAX_DELAY);
        if (res != pdTRUE)
        {
            printf("读取小数据队列失败!\n");
        }
        else
        {
            printf("读取小数据队列成功,键值:%d\n", key_value);
        }
    }
}

/**
 * @brief 大数据出队
 *
 * @param pvParameters
 */
void task3(void *pvParameters)
{
    char *buf;
    BaseType_t res;

    while (1)
    {
        res = xQueueReceive(big_date_queue, &buf, portMAX_DELAY);
        if (res != pdTRUE)
        {
            printf("读取大数据队列失败!\n");
        }
        else
        {
            printf("读取大数据队列成功,数据内容:%s\n", buf);
        }
    }
}
