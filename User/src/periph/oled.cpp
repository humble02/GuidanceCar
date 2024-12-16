//
// Created by 59794 on 2024/12/15.
//
#include "periph/oled.h"

#include <unordered_map>
#include <array>
#include <cstring>

using namespace periph;

void oled::init() {

    for (const auto& fontChar : F8X16) {
        fontMap[fontChar.symbol] = fontChar.bitmap;
    }

    uint8_t init_cmd[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0xA1, 0xC8, 0xDA,
        0x12, 0x81, 0xCF, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0x8D, 0x14, 0xAF
    };

    for (uint8_t cmd : init_cmd) {
        write_cmd(cmd);
    }
}

void oled::clear() {
    for (uint8_t i = 0; i < 128; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            setPos(i,j);
            write_data(0x00);
        }
    }
}

void oled::setPos(uint8_t seg, uint8_t page) {
    write_cmd(0x00 | (seg & (0x0f)));//seg low
    write_cmd(0x10 | (seg & (0xf0)>>4));//seg high
    write_cmd(0xb0 | page);//page
}

void oled::show(uint8_t seg, uint8_t page, char charactor) {
    setPos(seg, page);
    auto it = fontMap.find(charactor);
    if (it == fontMap.end()) {
        return;
    }
    const auto& bitmap = it->second;
    for (uint8_t i = 0; i < 8; i++) {
        write_data(bitmap[i]);  // Write each byte to the OLED display
    }
}

void oled::write_cmd(uint8_t cmd) {
    i2c_.SendData(oled_dev_address, oled_cmd_address, 8, &cmd, 1);
}
void oled::write_data(uint8_t data) {
    i2c_.SendData(oled_dev_address, oled_data_address, 8, &data, 1);
}




