#pragma once
#include <stdint.h>
#include "ImuEvent.h"

/**
 * @file ImuControl.h
 * @brief Public API for Doly IMU (Inertial Measurement Unit) control.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - Functions require init() before reading data
 * - State changes / updates are reported asynchronously through ImuEvent/ImuEventListener
 *
 * Threading notes:
 * - IMU update/gesture callbacks are typically invoked from an internal worker/event thread
 *
 * @ingroup doly_imucontrol
 */

namespace ImuControl
{
	/**
	 * @brief Initialize the IMU subsystem.
	 *
	 * This must be called once before reading IMU data or receiving events.
	 *
	 * @param delay Optional delay (milliseconds) before processing events.
	 * @param gx Gyroscope X offset.
	 * @param gy Gyroscope Y offset.
	 * @param gz Gyroscope Z offset.
	 * @param ax Accelerometer X offset.
	 * @param ay Accelerometer Y offset.
	 * @param az Accelerometer Z offset.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already active
	 * - -1 : init failed
	 * 
	 * @note Sampling rate 104hz (accel + gyro),  1Hz (temperature) 
	 */
	int8_t init(uint8_t delay = 0, int16_t gx = 0, int16_t gy = 0, int16_t gz = 0, int16_t ax = 0, int16_t ay = 0, int16_t az = 0);

	/**
	 * @brief Dispose/stop the IMU subsystem and release resources.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : not active
	 * - -1 : dispose failed
	 */
	int8_t dispose();

	/**
	 * @brief Calculate and output sensor offsets.
	 *
	 * Typically used to perform a calibration routine and retrieve offsets that can be
	 * passed to init() on later boots.
	 *
	 * @param gx Output gyroscope X offset.
	 * @param gy Output gyroscope Y offset.
	 * @param gz Output gyroscope Z offset.
	 * @param ax Output accelerometer X offset.
	 * @param ay Output accelerometer Y offset.
	 * @param az Output accelerometer Z offset.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : init failed
	 * - -2 : calculation failed
	 *
	 * @warning All output pointers must be valid (non-null).
	 */
	int8_t calculate_offsets(int16_t* gx, int16_t* gy, int16_t* gz, int16_t* ax, int16_t* ay, int16_t* az);

	/**
	 * @brief Get the last IMU reading snapshot.
	 *
	 * @return Latest ImuData captured by the IMU subsystem.
	 *
	 * @note init() must have been called successfully before meaningful data is available.
	 */
	ImuData getImuData();

	/**
	 * @brief Get the last temperature reading from the IMU.
	 * @return Temperature in degrees Celsius.
	 */
	float getTemperature();

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();

};
