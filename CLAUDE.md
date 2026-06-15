# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目名称

FreeRTOS_HAL — STM32F103 + FreeRTOS 学习工程

## 项目概述

基于 STM32F103ZET6（Cortex-M3）的 FreeRTOS V10.5.1 学习项目。使用 STM32CubeMX 生成 HAL 库底层代码，在 Keil MDK-ARM 环境下开发，通过逐个实验逐步掌握 FreeRTOS 内核功能。

## 技术栈

| 类别 | 技术 |
|---|---|
| **MCU** | STM32F103ZET6 (Cortex-M3, 72MHz, 512KB Flash, 64KB RAM) |
| **HAL 库** | STM32Cube FW_F1 V1.8.7 |
| **RTOS** | FreeRTOS V10.5.1 (抢占式调度，动态内存 heap_4) |
| **IDE / 编译器** | Keil MDK-ARM V5.32, ARM Compiler V5 (ARMCC) |
| **代码生成** | STM32CubeMX 6.12.1 |
| **调试输出** | USART1 (115200 8N1)，printf 重定向 |
| **调试器** | SWD (PA13/SWDIO, PA14/SWCLK) |

## 项目结构

```
FreeRTOS_HAL/
├── FreeRTOS_HAL.ioc              # CubeMX 硬件配置文件
├── Application/                  # FreeRTOS 应用层
│   ├── App_FreeRTOS.c            # 任务创建与应用逻辑
│   └── APP_FreeRTOS.h
├── BSP/                          # 板级支持包
│   ├── LED/                      # LED 驱动 (LED1=PA0, LED2=PA1, 低电平亮)
│   └── KEY/                      # 按键驱动 (KEY1=PF8, KEY2=PF9 轮询; KEY3=PF10 EXTI中断)
├── Core/                         # CubeMX 生成的硬件初始化
│   ├── Inc/                      # main.h, gpio.h, tim.h, usart.h, stm32f1xx_it.h
│   └── Src/                      # main.c, gpio.c, tim.c, usart.c, stm32f1xx_it.c, 启动文件等
├── Drivers/                      # STM32 HAL 库 & CMSIS
│   ├── CMSIS/
│   └── STM32F1xx_HAL_Driver/
├── FreeRTOS/                     # FreeRTOS 内核源码 & 配置
│   ├── FreeRTOSConfig.h          # 内核配置（剪裁、API 开关、中断映射）
│   ├── *.c                       # tasks.c, queue.c, timers.c, event_groups.c 等
│   ├── include/                  # FreeRTOS API 头文件
│   └── portable/                 # 移植层 (port.c, portmacro.h, heap_4.c)
└── MDK-ARM/                      # Keil 工程文件
    ├── FreeRTOS_HAL.uvprojx      # Keil 项目文件
    ├── FreeRTOS_HAL.uvoptx       # Keil 选项配置
    └── startup_stm32f103xe.s     # Cortex-M3 启动代码
```

## 外设配置

| 外设 | 引脚 | 用途 |
|---|---|---|
| GPIO | PA0 (LED1), PA1 (LED2) | LED 指示 |
| GPIO | PF8 (KEY1), PF9 (KEY2), PF10 (KEY3) | 按键输入 |
| USART1 | PA9 (TX), PA10 (RX) | printf 调试输出 (115200 8N1) |
| TIM1 | 内部 | HAL 时基 (1ms)，替代 SysTick |
| TIM6 | 内部 | 基本定时器，1s 周期，优先级 4 |
| TIM7 | 内部 | 基本定时器，1s 周期，优先级 6 |
| EXTI | PF10 (KEY3) | 上升沿触发，中断中恢复 FreeRTOS 任务 |

## FreeRTOS 关键配置

- **调度方式**: 抢占式 + 时间片轮转 (configUSE_PREEMPTION=1, configUSE_TIME_SLICING=1)
- **系统节拍**: 20Hz (每 tick 50ms)
- **最大优先级**: 5 级 (0~4)
- **堆内存**: 12KB (heap_4，动态分配)
- **最小任务栈**: 128 字
- **中断优先级分组**: NVIC_PRIORITYGROUP_4 (4 位抢占优先级)
- **受 FreeRTOS 管理的中断**: 优先级 5~15 可安全调用 API；优先级 0~4 不受内核管理
- **HAL 时基**: 使用 TIM1 而非 SysTick（SysTick 被 FreeRTOS 占用）
- **内存分配**: 仅动态分配 (configSUPPORT_STATIC_ALLOCATION 已注释)

## FreeRTOS 中断映射

```c
#define xPortPendSVHandler PendSV_Handler   // 任务切换
#define vPortSVCHandler    SVC_Handler      // 启动调度器
// SysTick_Handler 在 stm32f1xx_it.c 中条件调用 xPortSysTickHandler()
```

## 已完成的实验主题（按 git 提交顺序）

1. **任务创建与删除** — xTaskCreate / vTaskDelete，start_task 创建两个子任务后自删
2. **静态创建任务** — configSUPPORT_STATIC_ALLOCATION
3. **任务挂起与恢复** — vTaskSuspend / vTaskResume / xTaskResumeFromISR
4. **FreeRTOS 中断管理** — 中断优先级配置、临界区保护、ISR 中恢复任务

## 编码约定

- CubeMX 生成的代码位于 `USER CODE BEGIN/END` 注释块之间，重新生成时只有这些块内的代码会被保留
- BSP 驱动与 FreeRTOS 解耦，不直接依赖 RTOS API（KEY 的 EXTI 回调例外，在 App 层通过宏关联）
- 任务函数使用 `taskENTER_CRITICAL()` / `taskEXIT_CRITICAL()` 保护共享资源（如 printf/USART）
- 中文注释在部分编码环境下可能显示为乱码，建议新代码使用英文注释

## 注意事项

- 本工程**不适用** Makefile 或 CMake 构建，唯一构建入口是 Keil MDK-ARM
- printf 重定向到 USART1，依赖 `fputc()` 在 usart.c 中的重写实现
- 修改 `FreeRTOSConfig.h` 中的 config 宏后需重新编译全部 FreeRTOS 源文件
- TIM1 被配置为 HAL 时基（1ms 中断），TIM6/TIM7 为应用层 1s 定时器，修改时注意区分用途
- `MDK-ARM/` 目录下的 `.o`、`.axf`、`.map` 等编译产物已被 .gitignore 忽略
- 不要在 CubeMX 中重新生成工程时覆盖 `FreeRTOSConfig.h`（CubeMX 会生成默认配置）
- `HAL_Delay()` 依赖 TIM1 时基，在 FreeRTOS 任务中建议使用 `vTaskDelay()` 替代以实现非阻塞等待
