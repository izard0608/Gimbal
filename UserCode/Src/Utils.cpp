//
// Created by Izard on 2025/11/5.
//

#include "Utils.h"

float utils::linear_mapping(const float value, const float in_min, const float in_max, const float out_min, const float out_max)
{
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float utils::KalmanFilter::filter(float measure_value)
{
    now_P = last_P + Q;
    Kg = now_P / (now_P + R);
    output = output + Kg * (measure_value - output);
    last_P = (1 - Kg) * now_P;
    return output;
}
