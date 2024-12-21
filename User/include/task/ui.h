//
// Created by 59794 on 2024/12/15.
//

#ifndef UI_H
#define UI_H

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "main.h"
#include "periph/oled.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#ifdef __cplusplus
extern "C" {
#endif

    static void UI_Task(void* parameter);
    void UITaskStart();

#ifdef __cplusplus
}
#endif
#endif //UI_H
