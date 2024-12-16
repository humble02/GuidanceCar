//
// Created by 59794 on 2024/12/15.
//

#ifndef CHASSIS_H
#define CHASSIS_H

#include "FreeRTOS.h"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

    static void Chassis_Task(void* parameter);
    void ChassisTaskStart();

#ifdef __cplusplus
}
#endif

#endif //CHASSIS_H
