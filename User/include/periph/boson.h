//
// Created by 18759 on 2024/12/2.
//

#ifndef BOSON_H
#define BOSON_H
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "gpio_api.h"



namespace periph {
    class boson {
    public:
        boson();
        ~boson();
        void Detect() {
            result_ = gpio_.ReadPin();
        };
        [[nodiscard]] bool GetColor() const {
            return result_;
        }
    private:
        bsp::GPIO gpio_;
        bool result_;
    };
}

#endif
#endif //BOSON_H
