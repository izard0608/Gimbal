//=====================================================================================================
// MahonyAHRS.h
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
#ifndef MAHONY_AHRS_H
#define MAHONY_AHRS_H

//----------------------------------------------------------------------------------------------------
// Variable declaration

extern volatile float two_kp;			// 2 * proportional gain (Kp)
extern volatile float two_ki;			// 2 * integral gain (Ki)
extern volatile float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame

//---------------------------------------------------------------------------------------------------
// Function declarations

void mahony_ahr_supdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
void mahony_ahr_supdate_imu(float gx, float gy, float gz, float ax, float ay, float az);
void get_angle(float * yaw, float * pitch, float * roll);

#endif
//=====================================================================================================
// End of file
//=====================================================================================================