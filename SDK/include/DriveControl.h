#pragma once
#include <cstdint>
#include "DriveEventListener.h"
#include "Helper.h"

/**
 * @file DriveControl.h
 * @brief Public API for Doly drive motion control.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - Works only after successful init()
 * - Events are delivered via DriveEvent / DriveEventListener
 *
 * Threading notes:
 * - State changes and completion/error events are reported asynchronously
 *
 * @ingroup doly_drivecontrol
 */

namespace DriveControl
{
	/**
	 * @brief Initialize the drive control module.
	 *
	 * IMU offsets are calibration values stored by the platform.
	 *
	 * @param imu_off_gx IMU gyro X offset (calibration).
	 * @param imu_off_gy IMU gyro Y offset (calibration).
	 * @param imu_off_gz IMU gyro Z offset (calibration).
	 * @param imu_off_ax IMU accel X offset (calibration).
	 * @param imu_off_ay IMU accel Y offset (calibration).
	 * @param imu_off_az IMU accel Z offset (calibration).
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already active / already initialized
	 * - -1 : motor setup failed
	 * - -2 : IMU init failed
	 */
	int8_t init(int16_t imu_off_gx = 0, int16_t imu_off_gy = 0, int16_t imu_off_gz = 0,
		int16_t imu_off_ax = 0, int16_t imu_off_ay = 0, int16_t imu_off_az = 0);

	/**
	 * @brief Dispose/shutdown drive module and release resources.
	 *
	 * @param dispose_IMU If true, shuts down the IMU module as well.
	 *
	 * @return Status code:
	 * - 0 : success
	 * - 1 : not active / not initialized
	 */
	int8_t dispose(bool dispose_IMU);

	/**
	 * @brief Check whether the module is initialized and active.
	 * @return true if active, false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Abort current drive operation immediately.
	 *
	 * Typically used as an emergency stop for autonomous movement.
	 * Completion/error events may still be emitted depending on implementation.
	 */
	void Abort();

	/**
	 * @brief Manual motor drive (low-level).
	 *
	 * @param speed Motor speed percent (0..100).
	 * @param isLeft true = left motor, false = right motor.
	 * @param toForward true = forward, false = backward.
	 *
	 * @return true if accepted.
	 *
	 * @note Non-blocking; handled in another thread.
	 */
	bool freeDrive(uint8_t speed, bool isLeft, bool toForward);

	/**
	 * @brief Autonomous drive to an (x, y) target (high-level).
	 *
	 * @param id User command id (returned in events/callbacks).
	 * @param x Target X in your Position coordinate system (units are application-defined).
	 * @param y Target Y in your Position coordinate system (same unit as x).
	 * @param speed Requested speed percent (0..100).
	 * @param toForward Direction preference for the move.
	 * @param with_brake If true, brake at the end of motion.
	 * @param acceleration_interval Acceleration step interval (0 = disabled).
	 * @param control_speed Enable speed control dynamically.
	 * @param control_force Enable force/traction control (default true).
	 *
	 * @return true if command accepted.
	 *
	 * @note Non-blocking; handled in another thread.
	 */
	bool goXY(uint16_t id, int16_t x, int16_t y, uint8_t speed, bool toForward,
		bool with_brake = false, uint8_t acceleration_interval = 0,
		bool control_speed = false, bool control_force = true);

	/**
	 * @brief Autonomous drive for a fixed distance (high-level).
	 *
	 * @param id User command id (returned in events/callbacks).
	 * @param mm Distance value (named "mm" in API; ensure your implementation uses millimeters).
	 * @param speed Requested speed percent (0..100).
	 * @param toForward true forward / false backward.
	 * @param with_brake Brake at the end.
	 * @param acceleration_interval Acceleration step interval (0 = disabled).
	 * @param control_speed Enable speed control dynamically.
	 * @param control_force Enable force/traction control.
	 *
	 * @return true if command accepted.
	 *
	 * @note Non-blocking; handled in another thread.
	 */
	bool goDistance(uint16_t id, uint16_t mm, uint8_t speed, bool toForward,
		bool with_brake = false, uint8_t acceleration_interval = 0,
		bool control_speed = false, bool control_force = true);

	/**
	 * @brief Autonomous rotate (high-level).
	 *
	 * @param id User command id (returned in events/callbacks).
	 * @param rotateAngle Rotation angle in degrees (sign convention is implementation-defined).
	 * @param from_center true = rotate around center, false = rotate around a wheel/edge (implementation-defined).
	 * @param speed Requested speed percent (0..100).
	 * @param toForward Direction preference (implementation-defined for rotation).
	 * @param with_brake Brake at the end.
	 * @param acceleration_interval Acceleration step interval (0 = disabled).
	 * @param control_speed Enable speed control dynamically.
	 * @param control_force Enable force/traction control.
	 *
	 * @return true if command accepted.
	 *
	 * @note Non-blocking; handled in another thread.
	 */
	bool goRotate(uint16_t id, float rotateAngle, bool from_center, uint8_t speed, bool toForward,
		bool with_brake = false, uint8_t acceleration_interval = 0,
		bool control_speed = false, bool control_force = true);

	/**
	 * @brief Get current estimated position.
	 * @return Current Position estimate (see Helper.h for definition/units).
	 */
	Position getPosition();

	/**
	 * @brief Reset current position estimate to (0, 0, 0) (implementation-defined fields).
	 */
	void resetPosition();

	/**
	 * @brief Get current drive state.
	 * @return Current DriveState.
	 */
	DriveState getState();

	/**
	 * @brief Get current motor RPM.
	 * @param isLeft true = left motor, false = right motor.
	 * @return RPM value (units: revolutions per minute).
	 */
	float getRPM(bool isLeft);

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();
} // namespace DriveControl