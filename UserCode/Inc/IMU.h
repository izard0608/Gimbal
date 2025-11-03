//
// Created by Izard on 2025/11/3.
//

#ifndef GIMBAL_IMU_H
#define GIMBAL_IMU_H
#include <cstdint>

class IMU
{
public:
    void acc_calculate();
    void gyro_calculate();
    void get_angles();
private:
    uint8_t rx_acc_data[10] = {};
    uint8_t rx_gyro_data[10] = {};
    uint8_t raw_range = 0;
    float acc_value[3] = {};
    float gyro_value[3] = {};
    float ins_angle[3] = {0.0f, 0.0f, 0.0f};
};

#endif //GIMBAL_IMU_H