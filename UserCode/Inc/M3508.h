//
// Created by Izard on 2025/11/5.
//

#ifndef GIMBAL_M3508_H
#define GIMBAL_M3508_H

#include "Motor.h"

class M3508 : public Motor {
public:
    M3508(float ratio, uint16_t esc_id, CAN_HandleTypeDef * hcan, const PID & ppid, const PID & spid);
    void parse_can_msg_callback(const uint8_t rx_data[8]) final;
    void write_tx() final;
    float feedforward_intensity_calc(float current_angle) final;
};

#endif //GIMBAL_M3508_H