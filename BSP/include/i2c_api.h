#ifndef I2C_API_H
#define I2C_API_H
#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"
#include "main.h"

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

#include "callback.h"

namespace bsp {
    class I2C final {
    public:
        I2C_HandleTypeDef &handle_;
        I2C() = delete;
        explicit I2C(I2C_HandleTypeDef &handle): handle_(handle){};
        bool SendData(uint16_t dev_addr, uint16_t mem_addr,uint16_t add_size, uint8_t *data_buf, uint16_t len);
    };
}

#endif
#endif //I2C_API_H