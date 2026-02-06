#pragma once
#include <cstdint>

/**
 * @file DriveEventListener.h
 * @brief Public API types and listener interface for DriveControl.
 *
 * This header defines:
 * - public enums (error/state/type)
 * - the DriveEventListener interface used by DriveEvent
 *
 * @ingroup doly_drivecontrol
 */

 /**
  * @defgroup doly_drivecontrol DriveControl
  * @brief Doly drive motion control SDK (C++).
  *
  * DriveControl is exposed as a singleton-style API (namespace functions).
  * Events are delivered asynchronously through DriveEvent / DriveEventListener.
  * @{
  */

  /**
   * @brief Classification of a drive error.
   */
enum class DriveErrorType : std::uint8_t
{
	/** Operation was aborted before completion (manual stop or cancel). */
	ABORT,

	/** Excessive force / resistance detected. */
	FORCE,

	/** Rotation subsystem error. */
	ROTATE,

	/** Motor fault. */
	MOTOR,
};

/**
 * @brief Which motor side is associated with an error/event.
 */
enum class DriveMotorSide : std::uint8_t
{
	/** Both sides. */
	BOTH,

	/** Left side. */
	LEFT,

	/** Right side. */
	RIGHT,
};

/**
 * @brief High-level state of a drive operation.
 */
enum class DriveState : std::uint8_t
{
	/** Operation is in progress. */
	RUNNING,

	/** Operation completed successfully. */
	COMPLETED,

	/** Operation terminated due to an error. */
	ERROR,
};

/**
 * @brief Type/category of a drive operation.
 */
enum class DriveType : std::uint8_t
{
	/** Freeform/low-level control. */
	FREESTYLE,

	/** X/Y coordinated motion. */
	XY,

	/** Distance-based linear motion. */
	DISTANCE,

	/** Rotation-only motion. */
	ROTATE,
};

/**
 * @brief Interface for receiving drive events.
 *
 * Implement this interface and register it using DriveEvent::AddListener().
 *
 * @note Default implementations are no-ops, so you may override only what you need.
 * @warning Callbacks are typically invoked from an internal worker/event thread.
 *          Keep handlers fast and avoid blocking.
 * @warning The listener object must remain valid until it is removed via
 *          DriveEvent::RemoveListener().
 */
class DriveEventListener
{
public:
	virtual ~DriveEventListener() = default;

	/**
	 * @brief Called when a drive command completes successfully.
	 * @param id Identifier of the drive operation (provided by the caller).
	 */
	virtual void onDriveComplete(std::uint16_t id);

	/**
	 * @brief Called when a drive operation reports an error.
	 * @param id Identifier of the drive operation.
	 * @param side Which motor side the error occurred on.
	 * @param type Error classification.
	 */
	virtual void onDriveError(std::uint16_t id, DriveMotorSide side, DriveErrorType type);

	/**
	 * @brief Called when the state of a drive operation changes.
	 * @param driveType The kind of drive operation.
	 * @param state New state.
	 */
	virtual void onDriveStateChange(DriveType driveType, DriveState state);
};

/** @} */ // end of group doly_drivecontrol