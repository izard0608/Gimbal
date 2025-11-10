//
// Created by Izard on 2025/11/3.
//

#include "can.h"
#include "main.h"
#include "RemoteControl.h"
#include "tim.h"
#include "usart.h"
#include "M3508.h"
#include "M6020.h"

extern unsigned char rx_buf[20];
extern RemoteControl remote_control;

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
    // write later
}