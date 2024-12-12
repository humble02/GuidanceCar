//
// Created by 59794 on 2024/12/12.
//

#ifndef TEST_H
#define TEST_H

#include "FreeRTOS.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

    static void LED_Task(void* parameter);
    void LEDTaskStart();

#ifdef __cplusplus
}
#endif

#endif //TEST_H