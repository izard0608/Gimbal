//
// Created by Izard on 2025/11/5.
//

#ifndef GIMBAL_M3508_H
#define GIMBAL_M3508_H

#include "Motor.h"

class M3508 : public Motor {
public:
    M3508(float ratio, uint16_t esc_id, CAN_HandleTypeDef * hcan, const PID & ppid, const PID & spid) : Motor(ratio, esc_id, hcan, ppid, spid) {}
    void parse_can_msg_callback(const uint8_t rx_data[8]) final;
};

#endif //GIMBAL_M3508_H