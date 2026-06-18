#ifndef __KEY_H__
#define __KEY_H__

#include "gpio.h"

#define KEY_NOPRESS 0
#define KEY1_PRESS 1
#define KEY2_PRESS 2
#define KEY3_PRESS 3

void KEY_Init(void);
uint8_t KEY_SACN(void);

#endif /* __KEY_H__ */
