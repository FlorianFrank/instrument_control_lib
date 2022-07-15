//
// Created by florianfrank on 15.07.22.
//

#ifndef INSTRUMENT_CONTROL_LIB_KEI2600DEFINES_H
#define INSTRUMENT_CONTROL_LIB_KEI2600DEFINES_H

namespace SMU {

    /**
     * @brief Used to select which channel of the SMU should be used to measure or supplied.
     */
    enum SMU_CHANNEL {
        CHANNEL_A = 'a',
        CHANNEL_B = 'b'
    };

    /**
     * @brief Unit which should be changed or measured.
     */
    enum UNIT {
        VOLTAGE = 0,
        CURRENT = 1,
        POWER   = 2
    };
}
#endif //INSTRUMENT_CONTROL_LIB_KEI2600DEFINES_H
