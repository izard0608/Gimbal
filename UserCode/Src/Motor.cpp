//
// Created by Izard on 2025/11/3.
//
#include "Motor.h"
#include <cmath>
#include "Utils.h"
#include "can.h"

using namespace utils;

Motor::Motor(const float ratio, const uint16_t esc_id, CAN_HandleTypeDef *hcan, const PID & ppid, const PID & spid) :
    ratio_(ratio), esc_id_(esc_id), hcan_(hcan), can_tx_mailbox_(0), tx_header_(), ppid_(ppid), spid_(spid), rx_header_() {}

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
    if ((stop_flag_ == 1u) || fabsf(speed_.feedback) > 6000) {
        output_intensity_ = 0;
    }
    write_tx();
}

void Motor::read_motor_sensor(const CAN_HandleTypeDef *hcan)
{
    if (hcan->Instance == hcan_->Instance)
    {
        unsigned char rx_data[8];
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header_, rx_data);
        if (rx_header_.StdId == esc_id_)
        {
            parse_can_msg_callback(rx_data);
        }
    }
}