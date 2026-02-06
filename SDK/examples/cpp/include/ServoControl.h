#pragma once
#include <stdint.h>
#include "ServoEvent.h"

/**
 * @file ServoControl.h
 * @brief Public API for Doly servo control.
 *
 * This API controls IO port servo channels. Notifications for completion,
 * abort, and error can be observed via ServoEvent callbacks.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - init() must be called before setServo()
 * - setServo() is typically asynchronous; completion/abort/error are reported via ServoEvent
 *
 * @ingroup doly_servocontrol
 */

namespace ServoControl
{
	/**
	 * @brief Initialize the servo subsystem.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : SERVO_0 setup failed
	 * - -2 : SERVO_1 setup failed
	 */
	int8_t init();

	/**
	 * @brief Set a servo target angle.
	 *
	 * @param id User-defined action identifier (forwarded to event callbacks).
	 * @param channel Servo channel to control.
	 * @param angle Target angle (degrees).
	 * @param speed Speed percentage (valid range: 0..100).
	 * @param invert If true, invert the direction/orientation for the given channel.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : max angle exceed error
	 * - -2 : speed range error (0..100)
	 * - -3 : undefined channel
	 * - -4 : not initialized
	 */
	int8_t setServo(uint16_t id, ServoId channel, float angle, uint8_t speed, bool invert);

	/**
	 * @brief Abort an ongoing servo action.
	 * @param channel Servo channel to abort.
	 * @return Status code:
	 * - 0  : success
	 * - -1 : failed
	 */
	int8_t abort(ServoId channel);

	/**
	 * @brief Release servo hold (disable holding torque) for a channel.
	 *
	 * @param channel Servo channel to release.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : failed
	 * - -2 : busy/running
	 */
	int8_t release(ServoId channel);

	/**
	 * @brief Dispose/stop the servo subsystem and release resources.
	 * @return Status code (implementation-defined).
	 */
	int8_t dispose();

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();
};
