#pragma once
#include <stdint.h>
#include "Color.h"

/**
 * @file LedEventListener.h
 * @brief Listener interface and data types for LED control events and activities.
 *
 * This header defines:
 * - LED addressing enums (LedSide)
 * - LED activity state/enums (LedActivityState, LedErrorType)
 * - Activity descriptor (LedActivity)
 * - The LedEventListener callback interface used by LedEvent
 *
 * Threading notes:
 * - Callbacks invoked from an internal worker/event thread.
 * - Keep listener objects alive while they are registered (see LedEvent::AddListener()).
 *
 * @defgroup doly_ledcontrol LedControl
 * @brief Doly LED control and event API.
 * @{
 */

 /**
  * @brief Which LED(s) to address.
  */
enum class LedSide :uint8_t
{
	/** Apply to both sides (left and right). */
	BOTH,
	/** Left LED. */
	LEFT,
	/** Right LED. */
	RIGHT,
};

/**
 * @brief Activity lifecycle state.
 */
enum class LedActivityState :uint8_t
{
	/** No pending activity. */
	FREE,
	/** Activity is running (being processed by the worker). */
	RUNNING,
	/** Activity completed successfully. */
	COMPLETED
};

/**
 * @brief Error type reported by the LED subsystem.
 */
enum class LedErrorType :uint8_t
{
	/** Activity was aborted/cancelled. */
	ABORT,
};

/**
 * @brief Describes a single LED activity (color and fade parameters).
 *
 * Define @c mainColor and @c fadeColor as the same color to avoid fading.
 * In that case, @c fade_time behaves as a waiting time.
 */
struct LedActivity
{
	/** Start color. */
	Color mainColor;
	/** End color. */
	Color fadeColor;
	/** Fade time in milliseconds. */
	uint16_t fade_time = 0;
	/** Current activity state. */
	LedActivityState state = LedActivityState::FREE;
};

/**
 * @brief Observer interface for receiving LED completion and error events.
 *
 * Register an implementation using LedEvent::AddListener().
 *
 * @warning Do not destroy a listener object while it is registered.
 */
class LedEventListener
{
public:
	virtual ~LedEventListener() = default;

	/**
	 * @brief Called when an activity completes.
	 * @param id Activity identifier passed to LedControl::processActivity().
	 * @param side LED side associated with the completion.
	 */
	virtual void onLedComplete(uint16_t id, LedSide side);

	/**
	 * @brief Called when an activity fails or is aborted.
	 * @param id Activity identifier passed to LedControl::processActivity().
	 * @param side LED side associated with the error.
	 * @param type Error type.
	 */
	virtual void onLedError(uint16_t id, LedSide side, LedErrorType type);
};

/** @} */ // end of group doly_ledcontrol
