//
// Created by Izard on 2025/11/3.
//
#include "RemoteControl.h"
#include "stm32f4xx_hal.h"
#include "Utils.h"

using namespace utils;

bool RemoteControl::get_connection_status() const
{
    return HAL_GetTick() - signal_tick < 10;
}

void RemoteControl::update_signal_tick()
{
    signal_tick = HAL_GetTick();
}

void RemoteControl::get_rx_data(const uint8_t * rx_buf)
{
    for(int i = 0; i < 18; ++i) {
        rx_data[i] = rx_buf[i];
    }
    update_signal_tick();
}

void RemoteControl::handle()
{
    rc.ch0 = (rx_data[0] | rx_data[1] << 8) & 0x07FF;
    rc.ch1 = (rx_data[1] >> 3 | rx_data[2] << 5) & 0x07FF;
    rc.ch2 = (rx_data[2] >> 6 | rx_data[3] << 2 | rx_data[4] << 10) & 0x07FF;
    rc.ch3 = (rx_data[4] >> 1 | rx_data[5] << 7) & 0x07FF;

    rc.s1 = ((rx_data[5] >> 4) & 0x000C) >> 2;
    rc.s2 = (rx_data[5] >> 4) & 0x0003;
    rc.sw1 = static_cast<RcData::Sw>(rc.s1);
    rc.sw2 = static_cast<RcData::Sw>(rc.s2);

    rc.LeftAxisX = linear_mapping(rc.ch2, 364, 1684, -512, 512);
    rc.LeftAxisY = linear_mapping(rc.ch3, 364, 1684, -512, 512);
    rc.RightAxisX = linear_mapping(rc.ch0, 364, 1684, -512, 512);
    rc.RightAxisY = linear_mapping(rc.ch1, 364, 1684, -512, 512);
}

RemoteControl::RcData * RemoteControl::get_rc_data()
{
    return &rc;
}