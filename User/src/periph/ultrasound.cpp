//
// Created by 59794 on 2024/12/15.
//
#include "periph/ultrasound.h"

using namespace periph;

// Implementation
void Ultrasound::TriggerSignal() {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

bool Ultrasound::WaitForCaptureFlags() {
    uint32_t expire_time = HAL_GetTick() + kTimeoutMs;

    while (HAL_GetTick() < expire_time) {
        bool cc1_flag = __HAL_TIM_GET_FLAG(&timer_, TIM_FLAG_CC1);
        bool cc2_flag = __HAL_TIM_GET_FLAG(&timer_, TIM_FLAG_CC2);

        if (cc1_flag && cc2_flag) {
            return true;
        }
    }
    return false;
}

void Ultrasound::CalculateDistance() {
    uint32_t ccr1 = __HAL_TIM_GET_COMPARE(&timer_, TIM_CHANNEL_3);
    uint32_t ccr2 = __HAL_TIM_GET_COMPARE(&timer_, TIM_CHANNEL_4);

    float pulse_width = (ccr2 - ccr1) * 1e-6f;
    distance_ = 340.0f * pulse_width / 2.0f;
}

periph::periphState Ultrasound::Measure() {
    // Reset timer and flags
    __HAL_TIM_SET_COUNTER(&timer_, 0);
    __HAL_TIM_CLEAR_FLAG(&timer_, TIM_FLAG_CC1);
    __HAL_TIM_CLEAR_FLAG(&timer_, TIM_FLAG_CC2);

    // Start input capture
    HAL_TIM_IC_Start(&timer_, TIM_CHANNEL_3);
    HAL_TIM_IC_Start(&timer_, TIM_CHANNEL_4);

    // Trigger the ultrasound sensor
    TriggerSignal();

    // Wait for capture flags
    bool success = WaitForCaptureFlags();

    // Stop input capture
    HAL_TIM_IC_Stop(&timer_, TIM_CHANNEL_3);
    HAL_TIM_IC_Stop(&timer_, TIM_CHANNEL_4);

    if (success) {
        CalculateDistance();
        return periph::periphState::kOK;
    } else {
        return periph::periphState::kFail;
    }
}

float Ultrasound::GetDistance() const {
    return distance_;
}