//
// Created by Izard on 2025/11/3.
//

#include "UserTask.h"
#include "cmsis_os2.h"
#include "IMU.h"
#include "RemoteControl.h"
#include "Motor.h"

IMU imu;
RemoteControl remote_control;


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

        // rc data to motor status
        const RemoteControl::RcData * rc_status = remote_control.get_rc_data();
        if (rc_status->sw2 == RemoteControl::RcData::Sw::DOWN)
        {
            Motor::set_stop_flag();
        }
        if (rc_status->sw2 == RemoteControl::RcData::Sw::UP)
        {
            Motor::clear_stop_flag();
        }

        // imu calc
        imu.get_angles();

        // pid calc

        // can send

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