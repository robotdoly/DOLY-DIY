#pragma once
#include <stdint.h>

/**
 * @file TouchEventListener.h
 * @brief Listener interface and data types for touch sensor events.
 *
 * This header defines:
 * - Touch side selection (TouchSide)
 * - Touch state transitions (TouchState)
 * - Touch activity categories (TouchActivity)
 * - The TouchEventListener callback interface used by TouchEvent
 *
 * Threading notes:
 * - Callbacks invoked from an internal worker/event thread.
 * - Keep listener objects alive while they are registered (see TouchEvent::AddListener()).
 *
 * @defgroup doly_touchcontrol TouchControl
 * @brief Doly touch control and event API.
 * @{
 */

 /**
  * @brief Which touch sensor(s) to address.
  */
enum class TouchSide :uint8_t
{
	/** Apply to both sensors. */
	BOTH,
	/** Left touch sensor. */
	LEFT,
	/** Right touch sensor. */
	RIGHT
};

/**
 * @brief Touch press/release state.
 */
enum class TouchState : uint8_t
{
	/** Touch released. */
	UP,
	/** Touch pressed. */
	DOWN
};

/**
 * @brief High-level touch activity classification (implementation-defined).
 */
enum class TouchActivity : uint8_t
{
	/** Patting motion detected. */
	PATTING,
	/** Disturbing/standing touch detected. */
	DISTURB
};

/**
 * @brief Observer interface for receiving touch state and activity events.
 *
 * Register an implementation using TouchEvent::AddListener().
 *
 * @warning Do not destroy a listener object while it is registered.
 */
class TouchEventListener
{
public:
	virtual ~TouchEventListener() = default;

	/**
	 * @brief Called on touch state changes.
	 * @param side Which side triggered the event.
	 * @param state Current touch state.
	 */
	virtual void onTouchEvent(TouchSide side, TouchState state);

	/**
	 * @brief Called on detected touch activity (high-level pattern).
	 * @param side Which side triggered the activity.
	 * @param activity Activity classification.
	 */
	virtual void onTouchActivityEvent(TouchSide side, TouchActivity activity);
};

/** @} */ // end of group doly_touchcontrol
