//
// Created by 59794 on 2024/12/12.
//
//#include <cmath>
extern "C"{
#include "arm_math.h"
}
#include "periph/bmi088.h"
#include "dwt_api.h"
namespace imu {
    using namespace bsp;
    template<typename T>
    void  GetMaxandMinRange(T &input, T max, T min){
        if (input > max) {
            input = max;
        }
        if (input < min) {
            input = min;
        }
    }


    void Bmi088::Init() {
        Reset();
        calibrate = 1;
        ACCEL_SEN = reg::ACCEL_6G_SEN;
        GYRO_SEN = reg::GYRO_2000_SEN;

        error_ = Error::NO_ERROR;
        error_ = (TestAccel() != Error::NO_ERROR) ? Error::SELF_TEST_ACCEL_ERROR : InitAccel();
        error_ = (TestGyro() != Error::NO_ERROR) ? Error::SELF_TEST_GYRO_ERROR : InitGyro();
        GetOffset();
        state_ = error_ != Error::NO_ERROR ? State::STATE_LOST : State::STATE_CONNECTED;
    }

    void Bmi088::Reset() {
        accel_data_ = {0.0f, 0.0f, 0.0f};
        gyro_data_ = {0.0f, 0.0f, 0.0f};
        temperature = 0;
        caliOffset = 1;
        update_dt = 0;
        last_update_tick = 0;
    }

    Bmi088::Error Bmi088::TestAccel() {
        uint8_t res = 0;
        int16_t self_test_accel[2][3];
        uint8_t buff[6] = {0, 0, 0, 0, 0, 0};

        uint8_t &p_res = res;
        // check commiunication is normal

        ReadSingleReg(gpio_accel_, reg::ACC_CHIP_ID, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);
        ReadSingleReg(gpio_accel_, reg::ACC_CHIP_ID, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);

        // reset  bmi088 accel sensor and wait for > 50ms
        ReadSingleReg(gpio_accel_, reg::ACC_SOFTRESET, reg::ACC_SOFTRESET_VALUE);
        DWT_Delayms(reg::LONG_DELAY_TIME);

        // check commiunication is normal
        ReadSingleReg(gpio_accel_, reg::ACC_CHIP_ID, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);
        ReadSingleReg(gpio_accel_, reg::ACC_CHIP_ID, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);

        if (res != reg::ACC_CHIP_ID_VALUE) {
            error_ = Error::NO_SENSOR;
            return error_;
        }

        // set the accel register
        for (int write_reg_num = 0; write_reg_num < 4; write_reg_num++) {
            WriteSingleReg(gpio_accel_, reg::write_ACCEL_self_test_Reg_Data_Error[write_reg_num][0],
                           reg::write_ACCEL_self_test_Reg_Data_Error[write_reg_num][1]);
            DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);

            ReadSingleReg(gpio_accel_, reg::write_ACCEL_self_test_Reg_Data_Error[write_reg_num][0], res);
            DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);

            if (res != reg::write_ACCEL_self_test_Reg_Data_Error[write_reg_num][1]) {
                error_ = (Error) reg::write_ACCEL_self_test_Reg_Data_Error[write_reg_num][2];
                return error_;
            }

            // accel conf and accel range  . the two register set need wait for > 50ms
            DWT_Delayms(reg::LONG_DELAY_TIME);
        }

        // self test include postive and negative
        for (int write_reg_num = 0; write_reg_num < 2; write_reg_num++) {
            WriteSingleReg(gpio_accel_, reg::write_ACCEL_self_test_Reg_Data_Error[write_reg_num + 4][0],
                           reg::write_ACCEL_self_test_Reg_Data_Error[write_reg_num + 4][1]);
            DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);

            ReadSingleReg(gpio_accel_, reg::write_ACCEL_self_test_Reg_Data_Error[write_reg_num + 4][0], res);
            DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);

            if (res != reg::write_ACCEL_self_test_Reg_Data_Error[write_reg_num + 4][1]) {
                error_ = (Error) reg::write_ACCEL_self_test_Reg_Data_Error[write_reg_num + 4][2];
                return error_;
            }

            // accel conf and accel range  . the two register set need wait for > 50ms
            DWT_Delayms(reg::LONG_DELAY_TIME);

            // read response accel
            ReadMultiReg(gpio_accel_, reg::ACCEL_XOUT_L, buff, 6);

            self_test_accel[write_reg_num][0] = (int16_t) ((buff[1]) << 8) | buff[0];
            self_test_accel[write_reg_num][1] = (int16_t) ((buff[3]) << 8) | buff[2];
            self_test_accel[write_reg_num][2] = (int16_t) ((buff[5]) << 8) | buff[4];
        }

        // set self test off
        WriteSingleReg(gpio_accel_, reg::ACC_SELF_TEST, reg::ACC_SELF_TEST_OFF);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);
        ReadSingleReg(gpio_accel_, reg::ACC_SELF_TEST, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);

        if (res != (reg::ACC_SELF_TEST_OFF)) {
            error_ = Error::ACC_SELF_TEST_ERROR;
            return error_;
        }

        // reset the accel sensor
        WriteSingleReg(gpio_accel_, reg::ACC_SOFTRESET, reg::ACC_SOFTRESET_VALUE);
        DWT_Delayms(reg::LONG_DELAY_TIME);

        if ((self_test_accel[0][0] - self_test_accel[1][0] < 1365) ||
            (self_test_accel[0][1] - self_test_accel[1][1] < 1365) ||
            (self_test_accel[0][2] - self_test_accel[1][2] < 680)) {
            error_ = Error::SELF_TEST_ACCEL_ERROR;
            return error_;
        }

        ReadSingleReg(gpio_accel_, reg::ACC_CHIP_ID, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);
        ReadSingleReg(gpio_accel_, reg::ACC_CHIP_ID, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);

        return Error::NO_ERROR;
    }

    void Bmi088::ReadSingleReg(GPIO &gpio, uint8_t reg, uint8_t data) {
        gpio.Reset();
        spi_.SwapAByte((reg | 0x80));

        //@TODO ???
//        spi_.SwapAByte(0x55);

        data = spi_.SwapAByte(0x55);
        gpio.Set();
    }

    void Bmi088::WriteSingleReg(GPIO &gpio, uint8_t reg, uint8_t data) {
        gpio.Reset();
        spi_.SwapAByte(reg);
        spi_.SwapAByte(data);
        gpio.Set();
    }

    void Bmi088::ReadMultiReg(GPIO &gpio, uint8_t reg, uint8_t *data, uint16_t data_len) {
        gpio.Reset();
        spi_.SwapAByte((reg | 0x80));

        //@TODO ???
//        spi_.SwapAByte((reg | 0x80));

        spi_.ReadMultiReg(data, data_len);
        gpio.Set();
    }

    Bmi088::Error Bmi088::TestGyro() {
        uint8_t res = 0;
        uint8_t retry = 0;

        // check commiunication is normal
        ReadSingleReg(gpio_gyro_, reg::GYRO_CHIP_ID, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);
        ReadSingleReg(gpio_gyro_, reg::GYRO_CHIP_ID, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);

        // reset the gyro sensor
        WriteSingleReg(gpio_gyro_, reg::GYRO_SOFTRESET, reg::GYRO_SOFTRESET_VALUE);
        DWT_Delayms(reg::LONG_DELAY_TIME);

        // check commiunication is normal after reset
        ReadSingleReg(gpio_gyro_, reg::GYRO_CHIP_ID, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);
        ReadSingleReg(gpio_gyro_, reg::GYRO_CHIP_ID, res);
        DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);

        WriteSingleReg(gpio_gyro_, reg::GYRO_SELF_TEST, reg::GYRO_TRIG_BIST);
        DWT_Delayms(reg::LONG_DELAY_TIME);

        do {
            ReadSingleReg(gpio_gyro_, reg::GYRO_SELF_TEST, res);
            DWT_Delayms(reg::COM_WAIT_SENSOR_TIME);
            retry++;
        } while (!(res & reg::GYRO_BIST_RDY) && retry < 10);

        if (retry == 10) {
            error_ = Error::SELF_TEST_GYRO_ERROR;
            return error_;
        }
        if (res & reg::GYRO_BIST_FAIL) {
            error_ = Error::SELF_TEST_GYRO_ERROR;
            return error_;
        }
        return Error::NO_ERROR;
    }

    Bmi088::Error Bmi088::InitAccel() {
        volatile uint8_t res = 0;

        // check commiunication
        ReadSingleReg(gpio_accel_, reg::ACC_CHIP_ID, res);
        DWT_Delay(reg::COM_WAIT_SENSOR_TIME);
        ReadSingleReg(gpio_accel_, reg::ACC_CHIP_ID, res);
        DWT_Delay(reg::COM_WAIT_SENSOR_TIME);

        // accel software reset
        WriteSingleReg(gpio_accel_, reg::ACC_SOFTRESET, reg::ACC_SOFTRESET_VALUE);
        DWT_Delay(reg::LONG_DELAY_TIME);

        // check commiunication is normal after reset
        ReadSingleReg(gpio_accel_, reg::ACC_CHIP_ID, res);
        DWT_Delay(reg::COM_WAIT_SENSOR_TIME);
        ReadSingleReg(gpio_accel_, reg::ACC_CHIP_ID, res);
        DWT_Delay(reg::COM_WAIT_SENSOR_TIME);

        // check the "who am I"
        if (res != reg::ACC_CHIP_ID_VALUE) {
            return Error::NO_SENSOR;
        }

        // set accel sonsor config and check
        for (int write_reg_num = 0; write_reg_num < reg::WRITE_ACCEL_REG_NUM; write_reg_num++) {
            WriteSingleReg(gpio_accel_, reg::write_accel_reg_data_error[write_reg_num][0],
                           reg::write_accel_reg_data_error[write_reg_num][1]);
            DWT_Delay(reg::COM_WAIT_SENSOR_TIME);

            ReadSingleReg(gpio_accel_, reg::write_accel_reg_data_error[write_reg_num][0], res);
            DWT_Delay(reg::COM_WAIT_SENSOR_TIME);

            if (res != reg::write_accel_reg_data_error[write_reg_num][1]) {
                error_ = static_cast<Error>(reg::write_accel_reg_data_error[write_reg_num][2]);
                return error_;
            }
        }
        return Error::NO_ERROR;
    }

    Bmi088::Error Bmi088::InitGyro() {
        volatile uint8_t res = 0;

        // check commiunication
        ReadSingleReg(gpio_gyro_, reg::GYRO_CHIP_ID, res);
        DWT_Delay(reg::COM_WAIT_SENSOR_TIME);
        ReadSingleReg(gpio_gyro_, reg::GYRO_CHIP_ID, res);
        DWT_Delay(reg::COM_WAIT_SENSOR_TIME);

        // reset the gyro sensor
        WriteSingleReg(gpio_gyro_, reg::GYRO_SOFTRESET, reg::GYRO_SOFTRESET_VALUE);
        DWT_Delay(reg::LONG_DELAY_TIME);
        // check commiunication is normal after reset
        ReadSingleReg(gpio_gyro_, reg::GYRO_CHIP_ID, res);
        DWT_Delay(reg::COM_WAIT_SENSOR_TIME);
        ReadSingleReg(gpio_gyro_, reg::GYRO_CHIP_ID, res);
        DWT_Delay(reg::COM_WAIT_SENSOR_TIME);

        // check the "who am I"
        if (res != reg::GYRO_CHIP_ID_VALUE) {
            error_ = Error::NO_SENSOR;
            return error_;
        }

        // set gyro sonsor config and check
        for (int write_reg_num = 0; write_reg_num < reg::WRITE_GYRO_REG_NUM; write_reg_num++) {
            WriteSingleReg(gpio_gyro_, reg::write_gyro_reg_data_error[write_reg_num][0],
                           reg::write_gyro_reg_data_error[write_reg_num][1]);
            DWT_Delay(reg::COM_WAIT_SENSOR_TIME);

            ReadSingleReg(gpio_gyro_, reg::write_gyro_reg_data_error[write_reg_num][0], res);
            DWT_Delay(reg::COM_WAIT_SENSOR_TIME);

            if (res != reg::write_gyro_reg_data_error[write_reg_num][1]) {
                error_ = static_cast<Error>(reg::write_gyro_reg_data_error[write_reg_num][2]);
                return error_;
            }
        }
        return Error::NO_ERROR;
    }

    void Bmi088::SetOffset() {

        // offset
        float gyroMax[3], gyroMin[3];
        float gNormTemp, gNormMax, gNormMin, gNormDiff, gyroDiff[3];
        uint8_t buff[8] = {0, 0, 0, 0, 0, 0};
        static uint16_t CaliTimes = 6000;
        int16_t caliCount = 0;
        float start_time = DWT_GetTimeline_s();

        do {
            if (DWT_GetTimeline_s() - start_time > 10) {
                gyro_offset_[0] = reg::GxOFFSET;
                gyro_offset_[1] = reg::GyOFFSET;
                gyro_offset_[2] = reg::GzOFFSET;
                gNorm_ = reg::gNORM;
                break;
            }
            DWT_Delay(0.005);
            gNorm_ = 0;
            gyro_offset_[0] = 0;
            gyro_offset_[1] = 0;
            gyro_offset_[2] = 0;

            for (uint16_t i = 0; i < CaliTimes; i++) {
                ReadMultiReg(gpio_accel_, reg::ACCEL_XOUT_L, buff, 6);
                accel_data_.x = ((int16_t) ((buff[1]) << 8) | buff[0]) * ACCEL_SEN;
                accel_data_.y = ((int16_t) ((buff[3]) << 8) | buff[2]) * ACCEL_SEN;
                accel_data_.z = ((int16_t) ((buff[5]) << 8) | buff[4]) * ACCEL_SEN;

                gNormTemp = sqrtf(accel_data_.x * accel_data_.x +
                                  accel_data_.y * accel_data_.y +
                                  accel_data_.z * accel_data_.z);
                gNorm_ += gNormTemp;

                ReadMultiReg(gpio_gyro_, reg::GYRO_CHIP_ID, buff, 8);
                if (buff[0] == reg::GYRO_CHIP_ID_VALUE) {
                    gyro_data_.pitch = ((int16_t) ((buff[3]) << 8) | buff[2]) * GYRO_SEN;
                    gyro_data_.row = ((int16_t) ((buff[5]) << 8) | buff[4]) * GYRO_SEN;
                    gyro_data_.yaw = ((int16_t) ((buff[7]) << 8) | buff[6]) * GYRO_SEN;
                    gyro_offset_[0] += gyro_data_.pitch;
                    gyro_offset_[1] += gyro_data_.row;
                    gyro_offset_[2] += gyro_data_.yaw;
                }

                if (i == 0) {
                    gNormMax = gNormTemp;
                    gNormMin = gNormTemp;
                    gyroMax[0] = gyro_data_.pitch;
                    gyroMin[0] = gyro_data_.pitch;
                    gyroMax[1] = gyro_data_.row;
                    gyroMin[1] = gyro_data_.row;
                    gyroMax[2] = gyro_data_.yaw;
                    gyroMin[2] = gyro_data_.yaw;
                } else {
                    GetMaxandMinRange(gNormTemp, gNormMax, gNormMin);
                    GetMaxandMinRange(gyro_data_.pitch, gyroMax[0], gyroMin[0]);
                    GetMaxandMinRange(gyro_data_.row, gyroMax[1], gyroMin[1]);
                    GetMaxandMinRange(gyro_data_.yaw, gyroMax[2], gyroMin[2]);
                }

                gNormDiff = gNormMax - gNormMin;
                for (uint8_t j = 0; j < 3; j++) {
                    gyroDiff[j] = gyroMax[j] - gyroMin[j];
                }
                if (gNormDiff > 0.7f || gyroDiff[0] > 0.15f || gyroDiff[1] > 0.15f || gyroDiff[2] > 0.15f) {
                    break;
                }
                DWT_Delay(0.0005);
            }

            gNorm_ /= (float) CaliTimes;
            for (uint8_t i = 0; i < 3; i++) {
                gyro_offset_[i] /= (float) CaliTimes;
            }

            caliCount++;
        } while (gNormDiff > 0.7f ||
                 fabsf(gNorm_ - 9.8f) > 0.5f ||
                 gyroDiff[0] > 0.15f ||
                 gyroDiff[1] > 0.15f ||
                 gyroDiff[2] > 0.15f ||
                 fabsf(gyro_offset_[0]) > 0.01f ||
                 fabsf(gyro_offset_[1]) > 0.01f ||
                 fabsf(gyro_offset_[2]) > 0.01f);

        accel_scale_ = kGravity / gNorm_;
    }


    void Bmi088::GetOffset()
    {
        uint32_t offset_buf[4];
        float offset[4];
//        for (int i = 0; i < 4; i++)
//        {
//            uint8_t temp[4];
//            ui322buff(offset_buf[i], temp);
//            offset[i] = buff2float(temp);
//        }
//
//        if (offset[0] > 0.015f ||
//            offset[1] > 0.015f ||
//            offset[2] > 0.015f ||
//            offset[3] > 11.0f ||
//            offset[3] < 9.5f)
//        {
//            bmi088->gyro_offset[0] = BMI088_GxOFFSET;
//            bmi088->gyro_offset[1] = BMI088_GyOFFSET;
//            bmi088->gyro_offset[2] = BMI088_GzOFFSET;
//            bmi088->gNorm = BMI088_gNORM;
//            bmi088->accelScale = Gravity / bmi088->gNorm;
//        }
//        else
//        {
//            for (int i = 0; i < 3; i++)
//            {
//                bmi088->gyro_offset[i] = offset[i];
//            }
//
//            bmi088->gNorm = offset[3];
//            bmi088->accelScale = Gravity / bmi088->gNorm;
//        }
    }
}