//
// Created by Izard on 2025/11/3.
//
#include "Motor.h"
#include <cmath>
#include "Utils.h"
#include "can.h"

using namespace utils;

Motor::Motor(const float ratio, const uint16_t esc_id, CAN_HandleTypeDef *hcan, const PID & ppid, const PID & spid) :
    ratio_(ratio), esc_id_(esc_id), hcan_(hcan), can_tx_mailbox_(0), tx_header_(), ppid_(ppid), spid_(spid) {}

void Motor::clear_stop_flag()
{
    stop_flag_ = 0u;
}

void Motor::set_stop_flag()
{
    stop_flag_ = 1u;
}

void Motor::set_position(const float target_position, const float feedforward_speed, const float feedforward_intensity)
{
    angle_.target = target_position;
    feedforward_speed_ = feedforward_speed;
    feedforward_intensity_ = feedforward_intensity;
}

void Motor::set_speed(const float target_speed, const float feedforward_intensity)
{
    speed_.target = target_speed;
    feedforward_intensity_ = feedforward_intensity;
}

void Motor::set_intensity(const float intensity)
{
    output_intensity_ = intensity;
}

void Motor::handle()
{
    feedforward_intensity_ = feedforward_intensity_calc_(angle_.feedback);
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
    output_intensity_ = linear_mapping(clamp<float>(output_intensity_, -3.0f, 3.0f), -3.0f, 3.0f, -16384, 16384);
    // Protection
    if ((stop_flag_ == 1u) || fabsf(speed_.feedback) > 6000) {
        output_intensity_ = 0;
    }
    write_tx();
}

void Motor::read_motor_sensor(const CAN_HandleTypeDef *hcan, const CAN_RxHeaderTypeDef & rx_header, const uint8_t rx_data[8])
{
    if (rx_header.StdId == esc_id_ + 0x204)    // M6020: 0x204, M3508: 0x200
    {
        parse_can_msg_callback(rx_data);
    }
}

Motor::MotorState * Motor::get_motor_state()
{
    return &motor_state_;
}