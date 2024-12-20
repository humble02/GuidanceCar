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

    enum RoadSide {
        kLeftRoad = 0,
        kRightRoad = 1,
    };
    enum StateMachine {
        kTracking = 0,      //寻迹模式
        kAngle = 1,         //转直角模式
        kLeave = 2,         //避障远离模式
        kBack = 3,          //避障回归模式
        kRemote = 4,        //遥控
        kInit = 5           //初始化
    };

    enum TrackingState {
        kLeft = 0,
        kMidLeft = 1,
        kMiddle = 2,
        kMidRight = 3,
        kRight = 4
    };

    static void Chassis_Task(void* parameter);
    void ChassisTaskStart();

#ifdef __cplusplus
}
#endif

#endif //CHASSIS_H
