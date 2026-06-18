#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/***********************************************************************************************************
 *                                               基础配置
 ***********************************************************************************************************/
#define configUSE_PREEMPTION 1   // 置1：使用抢占式调度器 置0：使用协作式调度器
#define configUSE_TIME_SLICING 1 // 置1表示启用时间片调度
#define configUSE_IDLE_HOOK 0
#define configUSE_TICK_HOOK 0
#define configCPU_CLOCK_HZ ((unsigned long)72000000) // 写入实际的CPU内核时钟频率
#define configTICK_RATE_HZ ((TickType_t)1000)        // RTOS系统节拍中断频率，即每秒中断的次数
#define configMAX_PRIORITIES (5)                     // FreeRTOS支持的最高优先级
#define configMINIMAL_STACK_SIZE ((unsigned short)128)
#define configTOTAL_HEAP_SIZE ((size_t)(12 * 1024))
#define configMAX_TASK_NAME_LEN (16)
#define configUSE_TRACE_FACILITY 0
#define configUSE_16_BIT_TICKS 0
#define configIDLE_SHOULD_YIELD 1
#define configUSE_QUEUE_SETS 1          // 启用队列
#define configUSE_COUNTING_SEMAPHORES 1 // 启用计数型信号量
#define configUSE_MUTEXES 1             // 启用互斥信号量
#define configUSE_TASK_NOTIFICATIONS 1  // 启用任务通知

/***********************************************************************************************************
 *                                              内存申请配置
 ***********************************************************************************************************/
#define configSUPPORT_DYNAMIC_ALLOCATION 1 // 支持动态申请内存
#define configSUPPORT_STATIC_ALLOCATION 0  // 支持静态申请内存

/***********************************************************************************************************
 *                                              中断相关配置
 ***********************************************************************************************************/
#ifdef __NVIC_PRIO_BITS
#define configPRIO_BITS __NVIC_PRIO_BITS
#else
#define configPRIO_BITS 4
#endif
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15                                                                   // 中断最低优先级
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5                                                               // 系统可管理的最高中断优先级
#define configKERNEL_INTERRUPT_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))           // 对需要配置的SysTick与PendSV进行偏移
#define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS)) // 屏蔽优先级大于系统可管理的最高优先级中断
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY 15

/***********************************************************************************************************
 *                                              可选函数配置
 ***********************************************************************************************************/
#define INCLUDE_vTaskPrioritySet 1  // 设置任务优先级
#define INCLUDE_uxTaskPriorityGet 1 // 获取任务优先级
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskCleanUpResources 0
#define INCLUDE_vTaskSuspend 1 // 任务挂起
#define INCLUDE_vTaskDelayUntil 1
#define INCLUDE_vTaskDelay 1
#define INCLUDE_xTaskResumeFromISR 1     // 中断中恢复任务挂起
#define INCLUDE_xTaskGetSchedulerState 1 // 打开能够查询FreeRTOS调度状态

/***********************************************************************************************************
 *                                      FreeRTOS与中断服务函数有关配置
 ***********************************************************************************************************/
// 使用宏定义替代系统终端
#define xPortPendSVHandler PendSV_Handler
#define vPortSVCHandler SVC_Handler

#endif /* FREERTOS_CONFIG_H */
