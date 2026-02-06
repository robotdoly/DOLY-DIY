#pragma once
#include <cstdint>
#include <vector>
#include "ArmEvent.h"

/**
 * @file ArmControl.h
 * @brief Public API for Doly arm motion control.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - Functions require init() before use
 * - State changes are reported asynchronously through ArmEvent/ArmEventListener
 *
 * Threading notes:
 * - Events/callbacks are typically invoked from an internal worker/event thread
 *
 * @ingroup doly_armcontrol
 */

 /**
  * @brief Current arm angle data for one side.
  */
struct ArmData
{
	/** Which arm side this sample belongs to. */
	ArmSide side;

	/**
	 * @brief Angle in degrees.
	 *
	 * @note The definition of 0° and the direction of positive angles are implementation-defined.
	 */
	float angle;
};

namespace ArmControl
{
	/**
	 * @brief Initialize the arm subsystem.
	 *
	 * This must be called once before other control functions.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already initialized
	 * - -1 : left servo enable pin set failed
	 * - -2 : right servo enable pin set failed
	 *
	 * @note After successful init(), isActive() should return true.
	 */
	int8_t init();

	/**
	 * @brief Dispose/stop the arm subsystem and release resources.
	 *
	 * @return Status code:
	 * - 0 : success
	 * - 1 : not initialized
	 */
	int8_t dispose();

	/**
	 * @brief Check whether the subsystem is active (initialized and running).
	 * @return true if active, false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Abort the current operation for a given side.
	 *
	 * This is intended as an emergency/stop action.
	 *
	 * @param side Arm side to abort (LEFT/RIGHT/BOTH).
	 */
	void Abort(ArmSide side);

	/**
	 * @brief Get maximum allowed angle for the arm.
	 * @return Maximum angle (degrees).
	 */
	uint16_t getMaxAngle();

	/**
	 * @brief Command the arm to move to an angle.
	 *
	 * The operation is handled asynchronously (non-blocking). Results are reported via:
	 * - ArmEventListener::onArmComplete()
	 * - ArmEventListener::onArmError()
	 * - ArmEventListener::onArmStateChange()
	 *
	 * @param id User-defined command identifier (echoed back in completion/error callbacks).
	 * @param side Arm side to move (LEFT/RIGHT/BOTH).
	 * @param speed Speed percentage in range [1..100].
	 * @param angle Target angle in degrees in range [0..getMaxAngle()].
	 * @param with_brake If true, apply brake/hold behavior at target (implementation-defined).
	 *
	 * @return Status code:
	 * - 0  : success (command accepted)
	 * - -1 : not active (init() not called or subsystem not running)
	 * - -2 : speed out of range
	 * - -3 : angle out of range
	 *
	 * @warning This function is non-blocking; it does not wait for motion completion.
	 */
	int8_t setAngle(uint16_t id, ArmSide side, uint8_t speed, uint16_t angle, bool with_brake = false);

	/**
	 * @brief Get current operation state for a side.
	 * @param side Arm side.
	 * @return Current ArmState.
	 */
	ArmState getState(ArmSide side);

	/**
	 * @brief Get the current angle(s) for the requested side(s).
	 *
	 * @param side Requested side (LEFT/RIGHT/BOTH).
	 * @return Vector of ArmData entries. If side == BOTH, the vector may contain 2 elements.
	 */
	std::vector<ArmData> getCurrentAngle(ArmSide side);

	/**
	 * @brief Get SDK/library version.
	 *
	 * Format note from original header: 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();

} // namespace ArmControl