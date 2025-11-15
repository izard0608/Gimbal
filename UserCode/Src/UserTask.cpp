//
// Created by Izard on 2025/11/3.
//

#include "UserTask.h"
#include <cmath>
#include "cmsis_os2.h"
#include "IMU.h"
#include "RemoteControl.h"
#include "M3508.h"
#include "M6020.h"
#include "Utils.h"

using namespace utils;

IMU imu;
RemoteControl remote_control;
M6020 yaw();
M3508 pitch();
Motor::MotorState * pitch_motor_state, yaw_motor_state;


osThreadId_t main_control_handle;
constexpr osThreadAttr_t main_control_attributes {
    .name = "main_control",
    .stack_size = 512 * 4,
    .priority = osPriorityNormal,
};

[[noreturn]] void main_control(void *)   // rc data to motor pos, imu calc, pid calc, can send, 1000Hz
{
    for (;;)
    {
        const uint32_t last_wake_time = osKernelGetTickCount();

        // rc data to motor stop flag
        const RemoteControl::RcData * rc_status = remote_control.get_rc_data();
        float pitch_angle_delta = 0.0f, yaw_angle_delta = 0.0f;
        if (rc_status->sw2 == RemoteControl::RcData::Sw::DOWN)
        {
            Motor::set_stop_flag();
        }
        if (rc_status->sw2 == RemoteControl::RcData::Sw::UP)
        {
            Motor::clear_stop_flag();
        }

        // joystick to angle and deadzone check
        if (abs(rc_status->RightAxisX) > 20)
        {
            yaw_angle_delta = linear_mapping(rc_status->RightAxisX, -512, 512, -1.0f, 1.0f);
        }
        if (abs(rc_status->RightAxisY) > 20)
        {
            pitch_angle_delta = linear_mapping(rc_status->RightAxisY, -512, 512, -1.0f, 1.0f);
        }
        float pitch_target_angle = pitch_motor_state->ecd_angle, yaw_target_angle = yaw_motor_state.ecd_angle;
        if (pitch_target_angle + pitch_angle_delta > 30.0f) // mechanical limit
        {
            pitch_target_angle = 30.0f;
        }else
        {
            pitch_target_angle = pitch_motor_state->ecd_angle + pitch_angle_delta;
        }
        yaw_target_angle += yaw_angle_delta;
        yaw_target_angle = fmod(yaw_target_angle, 360.0f);

        // imu calc
        imu.get_angles();

        // pid calc, can send
        pitch.set_position(pitch_target_angle);
        yaw.set_position(yaw_target_angle);
        pitch.handle();
        yaw.handle();

        osDelayUntil(last_wake_time + 1);
    }
}

osThreadId_t rc_update_handle;
constexpr osThreadAttr_t rc_update_attributes {
    .name = "rc_update",
    .stack_size = 256 * 4,
    .priority = osPriorityNormal,
};

[[noreturn]] void rc_update(void *)   // rc data handling, 125Hz
{
    for (;;)
    {
        const uint32_t last_wake_time = osKernelGetTickCount();
        remote_control.handle();
        osDelayUntil(last_wake_time + 8);
    }
}

osThreadId_t imu_read_handle;
constexpr osThreadAttr_t imu_read_attributes {
    .name = "imu_read",
    .stack_size = 256 * 4,
    .priority = osPriorityNormal,
};

[[noreturn]] void imu_read(void *)
{
    for (;;)    // imu read, 1000Hz
    {
        const uint32_t last_wake_time = osKernelGetTickCount();
        imu.acc_calculate();
        imu.gyro_calculate();
        osDelayUntil(last_wake_time + 1);
    }
}


void user_task_init() {
    // User task initialization code goes here
    main_control_handle = osThreadNew(main_control, nullptr, &main_control_attributes);
    rc_update_handle = osThreadNew(rc_update, nullptr, &rc_update_attributes);
    imu_read_handle = osThreadNew(imu_read, nullptr, &imu_read_attributes);


}