//
// Created by Izard on 2025/11/5.
//
#include "M3508.h"
#include "Utils.h"

using namespace utils;

M3508::M3508(float ratio, uint16_t esc_id, CAN_HandleTypeDef * hcan, const PID & ppid, const PID & spid) : Motor(ratio, esc_id, hcan, ppid, spid)
{
    tx_header_ = {
        .StdId = 0x200,
        .ExtId = 0x000,
        .IDE = CAN_ID_STD,
        .RTR = CAN_RTR_DATA,
        .DLC = 8,
        .TransmitGlobalTime = DISABLE
    };
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

void M3508::write_tx()
{
    const uint8_t high_byte = static_cast<int16_t>(output_intensity_) >> 8;
    const uint8_t low_byte = static_cast<int16_t>(output_intensity_) & 0xFF;
    tx_data_[(esc_id_ - 1) * 2] = high_byte;
    tx_data_[(esc_id_ - 1) * 2 + 1] = low_byte;
    HAL_CAN_AddTxMessage(hcan_, &tx_header_, tx_data_, &can_tx_mailbox_);
}

float M3508::feedforward_intensity_calc(float current_angle)
{
    return 0.0f;
}
