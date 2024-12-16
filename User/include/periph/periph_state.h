//
// Created by 59794 on 2024/12/16.
//

#ifndef PERIPH_STATE_H
#define PERIPH_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus




namespace periph {
    enum class periphState {
        kOK = 0,
        kNoInit = 1,
        kFail = 2,
        kLoseConnect = 3,
    };
}

#endif

#endif //PERIPH_STATE_H
