#pragma once
#include <cstdint>

/**
 * @file ArmEventListener.h
 * @brief Public event types and listener interface for Doly ArmControl.
 *
 * This header defines:
 * - error/state/side enums
 * - the ArmEventListener interface used by the ArmEvent dispatcher
 *
 * @ingroup doly_armcontrol
 */

 /**
  * @defgroup doly_armcontrol ArmControl
  * @brief Doly arm motion control SDK (C++).
  *
  * ArmControl is exposed as a singleton-style API (namespace functions).
  * Events are delivered asynchronously through ArmEvent / ArmEventListener.
  * @{
  */

  /**
   * @brief Error categories reported by the arm subsystem.
   */
enum class ArmErrorType : uint8_t
{
	/** Operation aborted (user cancel / soft abort). */
	ABORT,

	/** Motor or driver related error. */
	MOTOR,
};

/**
 * @brief Which arm side an operation or event relates to.
 */
enum class ArmSide : uint8_t
{
	/** Both sides / whole-arm event. */
	BOTH,

	/** Left arm. */
	LEFT,

	/** Right arm. */
	RIGHT,
};

/**
 * @brief High-level state of an arm operation.
 */
enum class ArmState : uint8_t
{
	/** Operation is in progress. */
	RUNNING,

	/** Operation completed successfully. */
	COMPLETED,

	/** Operation terminated due to an error. */
	ERROR,
};

/**
 * @brief Interface for receiving arm events.
 *
 * Implement this interface and register it using ArmEvent::AddListener().
 *
 * @note Default implementations are no-ops, so you may override only what you need.
 * @warning Callbacks are typically invoked from an internal worker/event thread
 *          (not the caller thread). Keep handlers fast and avoid blocking.
 * @warning The listener object must remain valid until it is removed via
 *          ArmEvent::RemoveListener().
 */
class ArmEventListener
{
public:
	virtual ~ArmEventListener() = default;

	/**
	 * @brief Called when an arm command completes successfully.
	 * @param id Command identifier passed to ArmControl::setAngle().
	 * @param side Arm side for which the command completed.
	 */
	virtual void onArmComplete(uint16_t id, ArmSide side);

	/**
	 * @brief Called when an arm command ends with an error.
	 * @param id Command identifier passed to ArmControl::setAngle().
	 * @param side Arm side on which the error occurred.
	 * @param errorType Category of error.
	 */
	virtual void onArmError(uint16_t id, ArmSide side, ArmErrorType errorType);

	/**
	 * @brief Called when the arm state changes.
	 * @param side Arm side that changed state.
	 * @param state New state.
	 */
	virtual void onArmStateChange(ArmSide side, ArmState state);

	/**
	 * @brief Called to report incremental motion events.
	 *
	 * This is commonly used for “movement happened” notifications even when
	 * there is no active RUNNING command.
	 *
	 * @param side Arm side that moved.
	 * @param degreeChange Signed delta angle (degrees) since last notification.
	 */
	virtual void onArmMovement(ArmSide side, float degreeChange);
};

/** @} */ // end of group doly_armcontrol