//
// Created by Izard on 2025/11/3.
//

#include "../Inc/IMU.h"
#include <ctgmath>
#include "bmi088.h"
extern "C" {
#include "MahonyAHRS.h"
}
#define M_PI 3.14159265358979323846

void IMU::acc_calculate()
{
    bmi088_accel_read_reg(0x41, &raw_range, 1);
    bmi088_accel_read_reg(0x12, rx_acc_data, 6);
    acc_value[0] = static_cast<int16_t>((rx_acc_data[1] << 8) | rx_acc_data[0]) / 32768.0 * 1000 * pow(2, raw_range + 1) * 1.5;
    acc_value[1] = static_cast<int16_t>((rx_acc_data[3] << 8) | rx_acc_data[2]) / 32768.0 * 1000 * pow(2, raw_range + 1) * 1.5;
    acc_value[2] = static_cast<int16_t>((rx_acc_data[5] << 8) | rx_acc_data[4]) / 32768.0 * 1000 * pow(2, raw_range + 1) * 1.5;
}

void IMU::gyro_calculate()
{
    bmi088_gyro_read_reg(0x0F, &raw_range, 1);
    bmi088_gyro_read_reg(0x02, rx_gyro_data, 6);
    const int16_t gyro_range = 2000 / static_cast<int16_t>(pow(2, raw_range));
    gyro_value[0] = linear_mapping(static_cast<int16_t>((rx_gyro_data[1]) << 8) | rx_gyro_data[0], -32767, 32767, -gyro_range, gyro_range);
    gyro_value[1] = linear_mapping(static_cast<int16_t>((rx_gyro_data[3]) << 8) | rx_gyro_data[2], -32767, 32767, -gyro_range, gyro_range);
    gyro_value[2] = linear_mapping(static_cast<int16_t>((rx_gyro_data[5]) << 8) | rx_gyro_data[4], -32767, 32767, -gyro_range, gyro_range);
}

void IMU::get_angles()
{
    mahony_ahr_supdate_imu(gyro_value[0] / 180.0 * M_PI, gyro_value[1] / 180.0 * M_PI, gyro_value[2] / 180.0 * M_PI, acc_value[0], acc_value[1], acc_value[2]);
    get_angle(ins_angle, ins_angle + 1, ins_angle + 2);
}

IMU imu;