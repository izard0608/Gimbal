//
// Created by Izard on 2025/11/5.
//

#ifndef GIMBAL_M6020_H
#define GIMBAL_M6020_H

#include "Motor.h"

class M6020 final : public Motor {
public:
    M6020(float ratio, uint16_t esc_id, CAN_HandleTypeDef * hcan, const PID & ppid, const PID & spid);
    void parse_can_msg_callback(const uint8_t rx_data[8]) override;
    void write_tx() override;
    float feedforward_intensity_calc(float current_angle) override;
};

#endif //GIMBAL_M6020_H