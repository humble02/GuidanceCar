//
// Created by 18759 on 2024/12/2.
//

#ifndef TOYMOTOR_H
#define TOYMOTOR_H
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "pwm_api.h"
#include "gpio_api.h"
#include "periph_state.h"



namespace periph {
    class toyMotor {
    public:
        enum class Direction{
            kFront = 1,
            kBack = -1
        };
        toyMotor(bsp::PWMGenerator pwm, bsp::GPIO forward, bsp::GPIO backward)
        :forward_gpio_(forward), backward_gpio_(backward), speed_port_(pwm) {
            speed_ = 0.F;
            direction_ = Direction::kFront;
        }
        ~toyMotor();
        void init();
        void SetDir(Direction dir);
        void SetSpeed(float speed);
        periphState Output();
    private:
        bsp::GPIO forward_gpio_;
        bsp::GPIO backward_gpio_;
        bsp::PWMGenerator speed_port_;
        float speed_;
        Direction direction_;

    };
}

#endif
#endif //TOYMOTOR_H
