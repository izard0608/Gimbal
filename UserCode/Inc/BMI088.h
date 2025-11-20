//
// Created by Izard on 2025/11/3.
//

#ifndef GIMBAL_BMI088_H
#define GIMBAL_BMI088_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void bmi088_init();

void bmi088_write_byte(uint8_t tx_data);
void bmi088_read_byte(uint8_t *rx_data, uint8_t length);
void bmi088_write_reg(uint8_t reg, uint8_t data);

void bmi088_accel_ns_l();
void bmi088_accel_ns_h();
void bmi088_gyro_ns_l();
void bmi088_gyro_ns_h();

void bmi088_accel_write_single_reg(uint8_t reg, uint8_t data);
void bmi088_accel_read_reg(uint8_t reg, uint8_t *rx_data, uint8_t length); // 加速度计读取，注意需要忽略第一位数据dummy byte
void bmi088_gyro_read_reg(uint8_t reg, uint8_t *rx_data, uint8_t length);
void bmi088_gyro_write_single_reg(uint8_t reg, uint8_t tx_data);

#ifdef __cplusplus
}
#endif


#endif //GIMBAL_BMI088_H