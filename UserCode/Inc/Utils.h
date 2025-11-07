//
// Created by Izard on 2025/11/5.
//

#ifndef GIMBAL_UTIL_H
#define GIMBAL_UTIL_H

namespace utils
{
    float linear_mapping(float value, float in_min, float in_max, float out_min, float out_max);
    class KalmanFilter
    {
    public:
        float filter(float measure_value);
    private:
        float last_P = 0.02;
        float now_P = 0;
        float output = 0;
        float Kg = 0;
        float Q = 0.001;
        float R = 0.543;
    };
}



#endif //GIMBAL_UTIL_H