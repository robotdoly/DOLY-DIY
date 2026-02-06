#pragma once
#include <stdint.h>

/**
 * @file ServoEventListener.h
 * @brief Listener interface and data types for servo control events.
 *
 * This header defines:
 * - Servo channel identifiers (ServoId)
 * - The ServoEventListener callback interface used by ServoEvent
 *
 * Threading notes:
 * - Callbacks invoked from an internal worker/event thread.
 * - Keep listener objects alive while they are registered (see ServoEvent::AddListener()).
 *
 * @defgroup doly_servocontrol ServoControl
 * @brief Doly servo control and event API.
 * @{
 */

 /**
  * @brief Servo channel identifiers.
  */
enum class ServoId
{
	SERVO_0,
	SERVO_1,
};

/**
 * @brief Observer interface for receiving servo completion/abort/error events.
 *
 * Register an implementation using ServoEvent::AddListener().
 *
 * @warning Do not destroy a listener object while it is registered.
 */
class ServoEventListener
{
public:
	virtual ~ServoEventListener() = default;

	/**
	 * @brief Called when a servo action is aborted.
	 * @param id Action identifier passed to ServoControl::setServo().
	 * @param channel Servo channel.
	 */
	virtual void onServoAbort(uint16_t id, ServoId channel);

	/**
	 * @brief Called when a servo action fails.
	 * @param id Action identifier passed to ServoControl::setServo().
	 * @param channel Servo channel.
	 */
	virtual void onServoError(uint16_t id, ServoId channel);

	/**
	 * @brief Called when a servo action completes successfully.
	 * @param id Action identifier passed to ServoControl::setServo().
	 * @param channel Servo channel.
	 */
	virtual void onServoComplete(uint16_t id, ServoId channel);
};

/** @} */ // end of group doly_servocontrol
