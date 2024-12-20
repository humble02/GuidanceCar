//
// Created by 59794 on 2024/12/17.
//

#ifndef PROTO_PC_H
#define PROTO_PC_H

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

enum class Mode{
    kRemote = 0,
    kTracking = 1,
    kMapping = 2,
    kGuidance = 3,
};

enum class Move {
    kFront = 0,
    kBack = 1,
    kLeft = 2,
    kRight = 3,
};

enum class SpeedLevel {
    kFast = 0,
    kSlow = 1,
};

typedef struct{
    uint32_t  timestamp;
    uint8_t connect_;
    uint8_t move_;
    uint8_t mode_;
    uint8_t supercap_;
    uint8_t speed_;
    uint8_t stoptime_;

}PC2Board_t;

typedef struct{
  uint32_t  timestamp;
  uint8_t stopFlag_;
  uint8_t move_;

}Board2PC_t;

#endif

#endif //PROTO_PC_H
