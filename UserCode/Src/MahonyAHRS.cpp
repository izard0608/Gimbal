//=====================================================================================================
// MahonyAHRS.c
//=====================================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			Author			Notes
// 29/09/2011	SOH Madgwick    Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
//
//=====================================================================================================

//---------------------------------------------------------------------------------------------------
// Header files

#include "MahonyAHRS.h"
#include <math.h>

//---------------------------------------------------------------------------------------------------
// Definitions

#define SAMPLE_FREQ	1000.0f			// sample frequency in Hz
#define TWO_KP_DEF	(2.0f * 0.5f)	// 2 * proportional gain
#define TWO_KI_DEF	(2.0f * 0.0f)	// 2 * integral gain
#define M_PI 3.14159265358979323846

//---------------------------------------------------------------------------------------------------
// Variable definitions

volatile float two_kp = TWO_KP_DEF;											        // 2 * proportional gain (Kp)
volatile float two_ki = TWO_KI_DEF;											        // 2 * integral gain (Ki)
volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;					        // quaternion of sensor frame relative to auxiliary frame
volatile float integral_f_bx = 0.0f,  integral_f_by = 0.0f, integral_f_bz = 0.0f;	// integral error terms scaled by Ki

//---------------------------------------------------------------------------------------------------
// Function declarations

float inv_sqrt(float x);

//====================================================================================================
// Functions

//---------------------------------------------------------------------------------------------------
// AHRS algorithm update

void mahony_ahr_supdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz) {
	float recip_norm;

    // Use IMU algorithm if magnetometer measurement invalid (avoids NaN in magnetometer normalisation)
	if((mx == 0.0f) && (my == 0.0f) && (mz == 0.0f)) {
		mahony_ahr_supdate_imu(gx, gy, gz, ax, ay, az);
		return;
	}

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        // Normalise accelerometer measurement
		recip_norm = inv_sqrt(ax * ax + ay * ay + az * az);
		ax *= recip_norm;
		ay *= recip_norm;
		az *= recip_norm;

		// Normalise magnetometer measurement
		recip_norm = inv_sqrt(mx * mx + my * my + mz * mz);
		mx *= recip_norm;
		my *= recip_norm;
		mz *= recip_norm;

        // Auxiliary variables to avoid repeated arithmetic
        const float q0q0 = q0 * q0;
        const float q0q1 = q0 * q1;
        const float q0q2 = q0 * q2;
        const float q0q3 = q0 * q3;
        const float q1q1 = q1 * q1;
        const float q1q2 = q1 * q2;
        const float q1q3 = q1 * q3;
        const float q2q2 = q2 * q2;
        const float q2q3 = q2 * q3;
        const float q3q3 = q3 * q3;

        // Reference direction of Earth's magnetic field
        const float hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2));
        const float hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2q3 - q0q1));
        const float bx = sqrt(hx * hx + hy * hy);
        const float bz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f - q1q1 - q2q2));

		// Estimated direction of gravity and magnetic field
		float half_vx = q1q3 - q0q2;
		float half_vy = q0q1 + q2q3;
		float half_vz = q0q0 - 0.5f + q3q3;
        float half_wx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
        float half_wy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
        float half_wz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);

		// Error is sum of cross product between estimated direction and measured direction of field vectors
		float half_ex = (ay * half_vz - az * half_vy) + (my * half_wz - mz * half_wy);
		float half_ey = (az * half_vx - ax * half_vz) + (mz * half_wx - mx * half_wz);
		float half_ez = (ax * half_vy - ay * half_vx) + (mx * half_wy - my * half_wx);

		// Compute and apply integral feedback if enabled
		if(two_ki > 0.0f) {
			integral_f_bx += two_ki * half_ex * (1.0f / SAMPLE_FREQ);	// integral error scaled by Ki
			integral_f_by += two_ki * half_ey * (1.0f / SAMPLE_FREQ);
			integral_f_bz += two_ki * half_ez * (1.0f / SAMPLE_FREQ);
			gx += integral_f_bx;	// apply integral feedback
			gy += integral_f_by;
			gz += integral_f_bz;
		}
		else {
			integral_f_bx = 0.0f;	// prevent integral windup
			integral_f_by = 0.0f;
			integral_f_bz = 0.0f;
		}

		// Apply proportional feedback
		gx += two_kp * half_ex;
		gy += two_kp * half_ey;
		gz += two_kp * half_ez;
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / SAMPLE_FREQ));		// pre-multiply common factors
	gy *= (0.5f * (1.0f / SAMPLE_FREQ));
	gz *= (0.5f * (1.0f / SAMPLE_FREQ));
	const float qa = q0;
	const float qb = q1;
	const float qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	recip_norm = inv_sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recip_norm;
	q1 *= recip_norm;
	q2 *= recip_norm;
	q3 *= recip_norm;
}

//---------------------------------------------------------------------------------------------------
// IMU algorithm update

void mahony_ahr_supdate_imu(float gx, float gy, float gz, float ax, float ay, float az) {
	float recip_norm;

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
        // Normalise accelerometer measurement
		recip_norm = inv_sqrt(ax * ax + ay * ay + az * az);
		ax *= recip_norm;
		ay *= recip_norm;
		az *= recip_norm;

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		float half_vx = q1 * q3 - q0 * q2;
		float half_vy = q0 * q1 + q2 * q3;
		float half_vz = q0 * q0 - 0.5f + q3 * q3;

		// Error is sum of cross product between estimated and measured direction of gravity
		float half_ex = (ay * half_vz - az * half_vy);
		float half_ey = (az * half_vx - ax * half_vz);
		float half_ez = (ax * half_vy - ay * half_vx);

		// Compute and apply integral feedback if enabled
		if(two_ki > 0.0f) {
			integral_f_bx += two_ki * half_ex * (1.0f / SAMPLE_FREQ);	// integral error scaled by Ki
			integral_f_by += two_ki * half_ey * (1.0f / SAMPLE_FREQ);
			integral_f_bz += two_ki * half_ez * (1.0f / SAMPLE_FREQ);
			gx += integral_f_bx;	// apply integral feedback
			gy += integral_f_by;
			gz += integral_f_bz;
		}
		else {
			integral_f_bx = 0.0f;	// prevent integral windup
			integral_f_by = 0.0f;
			integral_f_bz = 0.0f;
		}

		// Apply proportional feedback
		gx += two_kp * half_ex;
		gy += two_kp * half_ey;
		gz += two_kp * half_ez;
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / SAMPLE_FREQ));		// pre-multiply common factors
	gy *= (0.5f * (1.0f / SAMPLE_FREQ));
	gz *= (0.5f * (1.0f / SAMPLE_FREQ));
	const float qa = q0;
	const float qb = q1;
	const float qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	recip_norm = inv_sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recip_norm;
	q1 *= recip_norm;
	q2 *= recip_norm;
	q3 *= recip_norm;
}

void get_angle(float * yaw, float * pitch, float * roll) {
    *yaw = atan2f(2.0f * (q1 * q2 + q0 * q3), 2.0f * (q0 * q0 + q1 * q1) - 1.0f) * 180.0f / M_PI;
    *pitch = asinf(-2.0f * (q1 * q3 - q0 * q2)) * 180.0f / M_PI;
    *roll = atan2f(2.0f * (q0 * q1 + q2 * q3), 2.0f * (q0 * q0 + q3 * q3) - 1.0f) * 180.0f / M_PI;
}

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root

float inv_sqrt(const float x) {
	const float half_x = 0.5f * x;
	float y = x;
	long i = *reinterpret_cast<long*>(&y);
	i = 0x5f3759df - (i>>1);
	y = *reinterpret_cast<float*>(&i);
	y = y * (1.5f - (half_x * y * y));
	return y;
}

//====================================================================================================
// END OF CODE
//====================================================================================================