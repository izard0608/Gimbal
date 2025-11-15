//
// Created by Izard on 2025/11/3.
//

#ifndef GIMBAL_REMOTECONTROL_H
#define GIMBAL_REMOTECONTROL_H

#include <cstdint>

class RemoteControl
{
public:
    bool get_connection_status() const;
    void handle();
    void update_signal_tick();
    void get_rx_data(const uint8_t * rx_buf);

    struct RcData
    {
        uint16_t ch0, ch1, ch2, ch3;
        int16_t LeftAxisX, LeftAxisY, RightAxisX, RightAxisY;
        uint8_t s1, s2;
        enum Sw
        {
            UP = 1, MID = 3, DOWN = 2
        }sw1, sw2;
    };
    RcData * get_rc_data();
private:
    unsigned char rx_data[20] = {};
    RcData rc = {};
    uint64_t signal_tick = 0;
};

#endif //GIMBAL_REMOTECONTROL_H