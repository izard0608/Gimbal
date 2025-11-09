//
// Created by Izard on 2025/11/3.
//

#include "UserTask.h"
#include "cmsis_os2.h"
#include "IMU.h"
#include "RemoteControl.h"

IMU imu;
RemoteControl remote_control;


osThreadId_t main_control_handle;
constexpr osThreadAttr_t main_control_attributes {
    .name = "main_control",
    .stack_size = 512 * 4,
    .priority = osPriorityNormal,
};

[[noreturn]] void main_control(void *arg)   // rc data to motor pos, imu calc, pid calc, can send, 1000Hz
{
    for (;;)
    {

    }
}

osThreadId_t rc_update_handle;
constexpr osThreadAttr_t rc_update_attributes {
    .name = "rc_update",
    .stack_size = 256 * 4,
    .priority = osPriorityNormal,
};

[[noreturn]] void rc_update(void *arg)   // rc data handling, 125Hz
{
    for (;;)
    {

    }
}

osThreadId_t imu_read_handle;
constexpr osThreadAttr_t imu_read_attributes {
    .name = "imu_read",
    .stack_size = 256 * 4,
    .priority = osPriorityNormal,
};

[[noreturn]] void imu_read(void *arg)
{
    for (;;)    // imu read, 1000Hz
    {

    }
}


void user_task_init() {
    // User task initialization code goes here
    main_control_handle = osThreadNew(main_control, nullptr, &main_control_attributes);
    rc_update_handle = osThreadNew(rc_update, nullptr, &rc_update_attributes);
}