//
// Created by Izard on 2025/11/5.
//
#include "M3508.h"

inline float linear_mapping(float value, float in_min, float in_max, float out_min, float out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void M3508::parse_can_msg_callback(const uint8_t rx_data[8])
{
    // Get Current EncoderAngle, Map to [0, 360)
    motor_state_.ecd_angle = linear_mapping(static_cast<float>(rx_data[0] << 8 | rx_data[1]), 0, 8191, 0, 360);
    if (motor_state_.init) {
        motor_state_.init = false;
        motor_state_.last_ecd_angle = motor_state_.ecd_angle;
    }
    // Get DeltaEncoderAngle, range [-180, 180)
    motor_state_.delta_ecd_angle = motor_state_.ecd_angle - motor_state_.last_ecd_angle;
    if (motor_state_.delta_ecd_angle < -180) {
        motor_state_.delta_ecd_angle += 360;
    }
    if (motor_state_.delta_ecd_angle > 180) {
        motor_state_.delta_ecd_angle -= 360;
    }
    // Accumulate Actual Angle
    motor_state_.last_ecd_angle = motor_state_.ecd_angle;
    motor_state_.delta_angle = motor_state_.delta_ecd_angle / ratio_;
    angle_.feedback += motor_state_.delta_angle;
    if (angle_.feedback > 360) {
        angle_.feedback -= 360;
    }
    if (angle_.feedback < 0) {
        angle_.feedback += 360;
    }

    // Get other information
    speed_.feedback = static_cast<int16_t>(rx_data[2] << 8 | rx_data[3]);
    motor_state_.current = linear_mapping(static_cast<int16_t>(rx_data[4] << 8 | rx_data[5]), -16384, 16384, -20, 20);
    motor_state_.temp = static_cast<float>(rx_data[6]);
}
