//
// Created by 59794 on 2024/12/12.
//
#include <cmsis_os.h>

#include "task/ins.h"
#include "dwt_api.h"
#include "periph/bmi088.h"
extern "C"{
#include "bsxlite_interface.h"
}

bsp::SPI imu_spi(hspi1);
bsp::GPIO accel_cs(*GPIOF, GPIO_PIN_3);
bsp::GPIO gyro_cs(*GPIOF, GPIO_PIN_4);
imu::Bmi088 ins(imu_spi, accel_cs, gyro_cs);

bsxlite_instance_t ins_instance;
vector_3d_t accel_data = {0, 0, 0};
vector_3d_t gyro_data = {0, 0, 0};

bsxlite_out_t bsx_out;
insData_t ins_data;

static void INS_Task(void* parameter)
{
    static bool init_flag{false};
    if (!init_flag) {
        bsp::DWT_Init(170);
        ins.Init();
        while(ins.error_ != imu::Bmi088::Error::NO_ERROR){
            ins.Init();
        }
        init_flag  = true;
    }
    bsxlite_init(&ins_instance);
    while (true)
    {
        ins.Decode();

        accel_data.x = ins.accel_data_.x;
        accel_data.y = ins.accel_data_.y;
        accel_data.z = ins.accel_data_.z;
        gyro_data.x = ins.gyro_data_.pitch;
        gyro_data.y = ins.gyro_data_.row;
        gyro_data.z = ins.gyro_data_.yaw;

        bsxlite_do_step(&ins_instance, 1000*HAL_GetTick(), &accel_data, &gyro_data, &bsx_out);

        ins_data = {
            0,
            0,
            0,
            bsx_out.orientation.yaw,
            bsx_out.orientation.pitch,
            bsx_out.orientation.roll};

        osDelay(10);
    }
}

const insData_t* GetInsData() {
    return &ins_data;
}

void InsTaskStart(void)
{
    xTaskCreate(INS_Task, "LEDTask", 256, NULL, 7, NULL);
}

