//
// Created by 18759 on 2024/11/23.
//

#ifndef FDCAN_H
#define FDCAN_H
#ifdef __cplusplus
extern "C" {
#endif

#include "fdcan.h"


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#include "callback.h"

namespace bsp {

    class FdcanTxConfig {
    public:
        FDCAN_TxHeaderTypeDef tx_header_{};
        uint8_t tx_data_[8]{0};

        explicit operator FDCAN_TxHeaderTypeDef() const {
            return tx_header_;
        }

        explicit FdcanTxConfig(uint32_t id, uint32_t dlc, uint32_t baud_rate_switch, uint32_t can_type);

        void Init(uint32_t id, uint32_t dlc, uint32_t baud_rate_switch, uint32_t can_type);
    };

    class Fdcan final : public Callback<3,void, uint8_t *,uint32_t, uint16_t> {
    public:
        FDCAN_HandleTypeDef *p_handle_;
        uint8_t tx_data_[16]{0};
        bool init_flag_{false};

        Fdcan() = delete;

        explicit Fdcan(FDCAN_HandleTypeDef *hfdcan) : p_handle_(hfdcan) {};

        void InitAndStart();

        void Send(FdcanTxConfig *p_config);

        void ErrorHandler();
    };


    void Init(FdcanTxConfig *p_config, uint32_t id, uint32_t dlc, uint32_t baud_rate_switch, uint32_t can_type);

    extern Fdcan fdcan1;
    extern Fdcan fdcan2;
    extern Fdcan fdcan3;
}


#endif
#endif //FDCAN_H
