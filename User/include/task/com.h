//
// Created by 59794 on 2024/12/20.
//

#ifndef PCCOM_H
#define PCCOM_H

#include "proto/proto_pc.h"

#ifdef __cplusplus
extern "C" {
#endif
    const uint8_t isConnect();
    const PC2Board_t* GetRxData();
    static void Com_Task(void* parameter);
    void ComTaskStart();

#ifdef __cplusplus
}
#endif

#endif //PCCOM_H
