//
// Created by 59794 on 2024/12/12.
//
#include "i2c_api.h"

using namespace bsp;

bool I2C::SendData(uint16_t dev_addr, uint16_t mem_addr,uint16_t add_size, uint8_t *data_buf, uint16_t len) {
    if(HAL_I2C_Mem_Write(&handle_, dev_addr, mem_addr,add_size, data_buf, len, 0x100)) {
        return true;
    }else {
        if(HAL_I2C_Mem_Write(&handle_, dev_addr, mem_addr,add_size, data_buf, len, 0x100)) {
            return true;
        }else {
            return false;
        }
    }
}
