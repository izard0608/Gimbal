//
// Created by Izard on 2025/11/3.
//
#include "Motor.h"
#include <cmath>

template<class T>
T clamp(const T & value, const T & min, const T & max)
{
    return value < min ? min : (value > max ? max : value);
}

Motor::Motor(const float ratio, const uint16_t esc_id, CAN_HandleTypeDef *hcan, const PID & ppid, const PID & spid) :
    ratio_(ratio),
    rx_header_(), hcan_(hcan), can_tx_mailbox_(0), ppid_(ppid), spid_(spid), esc_id_(esc_id)
{
}

void Motor::set_position(const float target_position, const float feedforward_speed, const float feedforward_intensity) {
    angle_.target = target_position;
    feedforward_speed_ = feedforward_speed;
    feedforward_intensity_ = feedforward_intensity;
}

void Motor::set_speed(const float target_speed, const float feedforward_intensity) {
    speed_.target = target_speed;
    feedforward_intensity_ = feedforward_intensity;
}

void Motor::set_intensity(const float intensity) {
    output_intensity_ = intensity;
}

void Motor::handle()
{
    feedforward_intensity_ = feedforward_intensity_calc(angle_.feedback);
    switch (control_method_) {
    case ControlMethod::TORQUE: {
            break;
    }
    case ControlMethod::SPEED: {
            output_intensity_ = spid_.calc(speed_.target, speed_.feedback) + feedforward_intensity_;
            break;
    }
    case ControlMethod::POSITION_SPEED: {
            speed_.target = ppid_.calc(angle_.target, angle_.feedback) + feedforward_speed_;
            output_intensity_ = spid_.calc(speed_.target, speed_.feedback) + feedforward_intensity_;
    }
    }
    output_intensity_ = clamp<int16_t>(static_cast<int16_t>(output_intensity_), -16384, 16384);
    // Protection
    if ((stop_flag_ != 0u) || fabsf(speed_.feedback) > 6000) {
        output_intensity_ = 0;
    }
    write_tx();
}

void Motor::write_tx()
{
    const uint8_t high_byte = static_cast<int16_t>(output_intensity_) >> 8;
    const uint8_t low_byte = static_cast<int16_t>(output_intensity_) & 0x00FF;
    tx_data_[0] = tx_data_[2] = tx_data_[4] = tx_data_[6] = high_byte;
    tx_data_[1] = tx_data_[3] = tx_data_[5] = tx_data_[7] = low_byte;
    HAL_CAN_AddTxMessage(hcan_, &tx_header_, tx_data_, &can_tx_mailbox_);
}