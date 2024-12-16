//
// Created by 59794 on 2024/12/12.
//
#include <cmsis_os.h>

#include "task/test.h"
#include "gpio_api.h"
#include "pwm_api.h"
#include "dwt_api.h"
#include "periph/bmi088.h"

// bsp::GPIO test_pin(*GPIOG, GPIO_PIN_8);
//
// void Bulb() {
//     test_pin.Set();
// }

int debug_left = 0;
int debug_right = 0;

bsp::GPIO left_f(*GPIOG, GPIO_PIN_5);
bsp::GPIO left_b(*GPIOG, GPIO_PIN_6);
bsp::GPIO right_f(*GPIOG, GPIO_PIN_7);
bsp::GPIO right_b(*GPIOG, GPIO_PIN_8);

bsp::PWMGenerator left_en(&htim3, TIM_CHANNEL_1);
bsp::PWMGenerator right_en(&htim3, TIM_CHANNEL_2);

void MotorRun() {

}

static void LED_Task(void* parameter)
{

    left_en.Init();
    right_en.Init();
    left_en.Start();
    right_en.Start();
    while (true)
    {
         if(debug_left == 0) {
             left_f.Set();
             left_b.Set();
         }else {
             left_en.SetDuty(50000.F * fabs(debug_left)/100);
             if(debug_left < 0) {
                 left_f.Set();
                 left_b.Reset();
             }else {
                 left_f.Reset();
                 left_b.Set();
             }
         }
         if(debug_left == 0) {
             right_f.Set();
             right_b.Set();
         }else {
             right_en.SetDuty(50000.F * fabs(debug_right)/100);
             if(debug_right < 0) {
                 right_f.Set();
                 right_b.Reset();
             }else {
                 right_f.Reset();
                 right_b.Set();
             }
         }
         left_en.Output();
         right_en.Output();

        osDelay(1);
    }
}
void LEDTaskStart(void)
{
    xTaskCreate(LED_Task, "LEDTask", 256, NULL, 7, NULL);
}