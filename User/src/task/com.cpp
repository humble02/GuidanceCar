//
// Created by 59794 on 2024/12/20.
//
#include <cmsis_os.h>
#include "task/com.h"

#include "proto/proto_pc.h"
#include "uart_api.h"
#include <cstring>

Board2PC_t txMsg;
PC2Board_t rxMsg;
bsp::Uart pc_port(&huart1);
bsp::UartTxConfig Transmit(reinterpret_cast<uint8_t *>(&txMsg), sizeof(txMsg), 1000);
bool connect_on = true;
uint32_t last_stamp = 0;

uint8_t* PC_CallBack(uint16_t num) {
    last_stamp = rxMsg.timestamp;
    memcpy(&rxMsg, pc_port.rx_data_, sizeof(rxMsg));
    if (rxMsg.timestamp == last_stamp) {
        connect_on = false;
    }
    return pc_port.rx_data_;
}

void ComInit() {
    pc_port.InitAndStart();
    pc_port.AddCallback(PC_CallBack);
}

void SetTxData() {

    Transmit.Init(reinterpret_cast<uint8_t *>(&txMsg), sizeof(txMsg), 1000);
}

void DataSend() {
    pc_port.Send(&Transmit);
}

const bool isConnect() {
    return connect_on;
}

const PC2Board_t* GetRxData() {
    return &rxMsg;
}

static void Com_Task(void* parameter)
{

    ComInit();

    while (true)
    {
        if (connect_on) {
            SetTxData();
            DataSend();
        }

        osDelay(1);
    }
}
void ComTaskStart(void)
{
    xTaskCreate(Com_Task, "ComTask", 256, NULL, 7, NULL);
}




