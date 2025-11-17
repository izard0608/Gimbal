//
// Created by Izard on 2025/11/3.
//

#include "can.h"
#include "main.h"
#include "RemoteControl.h"
#include "usart.h"
#include "M6020.h"
#include "iwdg.h"

extern unsigned char rx_buf[20];
extern RemoteControl remote_control;
extern M6020 pitch;
extern M6020 yaw;
extern Motor::MotorState * pitch_motor_state, yaw_motor_state;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART3)  // read rc
    {
        HAL_UART_Transmit_DMA(&huart3, rx_buf, 18);
        remote_control.get_rx_data(rx_buf);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_buf, 18);
    }
}

void hal_can_rx_fifo0_msg_pending_callback(CAN_HandleTypeDef *hcan)
{
    if (hcan == &hcan1)
    {
        uint8_t rx_data[8];
        CAN_RxHeaderTypeDef rx_header;
        HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header, rx_data);
        pitch.read_motor_sensor(hcan, rx_header, rx_data);
        yaw.read_motor_sensor(hcan, rx_header, rx_data);
    }
}
