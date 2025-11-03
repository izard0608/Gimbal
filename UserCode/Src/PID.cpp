//
// Created by Izard on 2025/11/3.
//

#include "PID.h"
#include <algorithm>

PID::PID(const float kp, const float ki, const float kd, const float i_max, const float out_max, const float d_filter_k) {
    kp_ = kp, ki_ = ki, kd_ = kd;
    i_max_ = i_max, out_max_ = out_max;
    d_filter_k_ = d_filter_k;

    reset();
}
// Mutable Parameters
void PID::reset() {
    output_ = 0;
    ref_ = fdb_ = 0;
    err_ = err_sum_ = last_err_ = 0;
    pout_ = iout_ = dout_ = last_dout_ = 0;
}

float PID::calc(const float ref, const float fdb) {
    // Update Status
    ref_ = ref, fdb_ = fdb;
    last_err_ = err_;
    err_ = ref_ - fdb_;
    err_sum_ += err_;
    last_dout_ = dout_;

    // Calculate Output
    pout_ = err_ * kp_;
    iout_ = std::min<float>(err_sum_ * ki_, i_max_);
    dout_ = (err_ - last_err_) * kd_;
    // Apply D-Filter
    dout_ = last_dout_ + d_filter_k_ * (dout_ - last_dout_);

    output_ = std::min<float>(out_max_, pout_ + iout_ + dout_);
    return output_;
}