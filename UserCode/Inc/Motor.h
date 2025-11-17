//
// Created by Izard on 2025/11/3.
//

#ifndef GIMBAL_MOTOR_H
#define GIMBAL_MOTOR_H
#include "PID.h"
#include "can.h"

class Motor
{
public:
    enum class ControlMethod
    {
        TORQUE = 0,
        SPEED = 1,
        POSITION_SPEED = 2
    }control_method_ = ControlMethod::TORQUE;

    Motor(float ratio, uint16_t esc_id, CAN_HandleTypeDef * hcan, const PID & ppid, const PID & spid);
    virtual ~Motor() = default;

    static void clear_stop_flag();
    static void set_stop_flag();

    virtual void parse_can_msg_callback(const uint8_t rx_data[8]) = 0;
    virtual void write_tx() = 0;

    float (*feedforward_intensity_calc_)(float current_angle) = [](float) -> float {return 0.0f; };


    void set_position(float target_position, float feedforward_speed = 0.0f, float feedforward_intensity = 0.0f);
    void set_speed(float target_speed, float feedforward_intensity = 0.0f);
    void set_intensity(float intensity);

    void handle();
    virtual void read_motor_sensor(const CAN_HandleTypeDef *hcan);

    struct MotorState
    {
        float delta_angle = 0;
        float ecd_angle = 0;
        float last_ecd_angle = 0;
        float delta_ecd_angle = 0;
        float current = 0;
        float temp = 0;
        bool init = true; // delta = 0 when first read
    };
    MotorState * get_motor_state();

protected:
    const float ratio_;

    uint16_t esc_id_;

    MotorState motor_state_;

    float feedforward_intensity_ = 0;
    float output_intensity_ = 0;
    float feedforward_speed_ = 0;

    struct PIDControllerParam
    {
        float target = 0;
        float feedback = 0;
    }speed_, angle_;

    CAN_HandleTypeDef *hcan_ = nullptr;

    uint8_t tx_data_[8]{};
    uint32_t can_tx_mailbox_;
    CAN_TxHeaderTypeDef tx_header_;
private:
    PID ppid_, spid_;
    inline static uint8_t stop_flag_ = 1;
    CAN_RxHeaderTypeDef rx_header_;
};

#endif //GIMBAL_MOTOR_H