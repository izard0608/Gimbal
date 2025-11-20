//
// Created by Izard on 2025/11/3.
//
#include "BMI088.h"
#include "spi.h"

void bmi088_init() {
    // Soft Reset ACCEL
    bmi088_accel_ns_l();
    bmi088_write_reg(0x7E, 0xB6); // Write 0xB6 to ACC_SOFTRESET(0x7E)
    HAL_Delay(1);
    bmi088_accel_ns_h();

    // Soft Reset GYRO
    bmi088_gyro_ns_l();
    bmi088_write_reg(0x14, 0xB6); // Write 0xB6 to GYRO_SOFTRESET(0x14)
    HAL_Delay(30);
    bmi088_gyro_ns_h();

    // Switch ACCEL to Normal Mode
    bmi088_accel_ns_l();
    HAL_Delay(1);
    bmi088_write_reg(0x7D, 0x04); // Write 0x04 to ACC_PWR_CTRL(0x7D)
    HAL_Delay(1);
    bmi088_accel_ns_h();
}
void bmi088_write_byte(const uint8_t tx_data) {
    HAL_SPI_Transmit(&hspi1, &tx_data, 1, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX) {}
}

void bmi088_read_byte(uint8_t *rx_data, const uint8_t length)
{
    HAL_SPI_Receive(&hspi1, rx_data, length, 1000);
    while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_RX) {}
}

void bmi088_write_reg(const uint8_t reg, const uint8_t data)
{
    bmi088_write_byte(reg & 0x7F);
    bmi088_write_byte(data);
}

void bmi088_accel_ns_l() {
    HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_RESET);
}
void bmi088_accel_ns_h() {
    HAL_GPIO_WritePin(CS1_ACCEL_GPIO_Port, CS1_ACCEL_Pin, GPIO_PIN_SET);
}
void bmi088_gyro_ns_l()
{
    HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_RESET);
}
void bmi088_gyro_ns_h()
{
    HAL_GPIO_WritePin(CS1_GYRO_GPIO_Port, CS1_GYRO_Pin, GPIO_PIN_SET);
}

void bmi088_accel_write_single_reg(const uint8_t reg, const uint8_t data)
{
    bmi088_gyro_ns_h();
    bmi088_accel_ns_l();
    bmi088_write_reg(reg, data);
    bmi088_accel_ns_h();
}
void bmi088_accel_read_reg(const uint8_t reg, uint8_t *rx_data, const uint8_t length) // 加速度计读取，注意需要忽略第一位数据dummy byte
{
    bmi088_gyro_ns_h();
    bmi088_accel_ns_l();
    bmi088_write_byte(reg | 0x80);
    bmi088_read_byte(rx_data, 1);
    bmi088_read_byte(rx_data, length);
    bmi088_accel_ns_h();
}
void bmi088_gyro_write_single_reg(const uint8_t reg, const uint8_t tx_data)
{
    bmi088_accel_ns_h();
    bmi088_gyro_ns_l();
    bmi088_write_reg(reg, tx_data);
    bmi088_gyro_ns_h();
}
void bmi088_gyro_read_reg(const uint8_t reg, uint8_t *rx_data, const uint8_t length)
{
    bmi088_accel_ns_h();
    bmi088_gyro_ns_l();
    bmi088_write_byte(reg | 0x80);
    bmi088_read_byte(rx_data, length);
    bmi088_gyro_ns_h();
}
