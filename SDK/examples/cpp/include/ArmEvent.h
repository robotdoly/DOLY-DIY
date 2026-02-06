#pragma once
#include <cstdint>
#include "ArmEventListener.h"

/**
 * @file ArmEvent.h
 * @brief Event dispatcher API for the Doly ArmControl subsystem.
 *
 * ArmEvent delivers notifications to:
 * - registered ArmEventListener objects
 * - optional C-style function callbacks
 *
 * @ingroup doly_armcontrol
 */

namespace ArmEvent
{
	/**
	 * @brief Register a listener object to receive arm events.
	 *
	 * @param observer Pointer to a listener instance (must not be null).
	 * @param priority If true, the listener is inserted with priority ordering (implementation-defined).
	 *
	 * @warning The listener object must remain valid until removed with RemoveListener().
	 * @note Callbacks are typically invoked from an internal worker/event thread.
	 */
	void AddListener(ArmEventListener* observer, bool priority = false);

	/**
	 * @brief Unregister a listener object.
	 * @param observer Pointer previously passed to AddListener().
	 */
	void RemoveListener(ArmEventListener* observer);

	/**
	 * @brief Register a C-style callback for “command complete” events.
	 * @param onEvent Function pointer called with (id, side).
	 *
	 * @warning The callback is invoked asynchronously (often from a worker/event thread).
	 * @note You can register multiple callbacks (implementation-defined).
	 */
	void AddListenerOnComplete(void(*onEvent)(uint16_t id, ArmSide side));

	/**
	 * @brief Remove a previously registered “command complete” callback.
	 * @param onEvent Same function pointer passed to AddListenerOnComplete().
	 */
	void RemoveListenerOnComplete(void(*onEvent)(uint16_t id, ArmSide side));

	/**
	 * @brief Register a C-style callback for error events.
	 * @param onEvent Function pointer called with (id, side, errorType).
	 *
	 * @warning The callback is invoked asynchronously (often from a worker/event thread).
	 */
	void AddListenerOnError(void(*onEvent)(uint16_t id, ArmSide side, ArmErrorType errorType));

	/**
	 * @brief Remove a previously registered error callback.
	 * @param onEvent Same function pointer passed to AddListenerOnError().
	 */
	void RemoveListenerOnError(void(*onEvent)(uint16_t id, ArmSide side, ArmErrorType errorType));

	/**
	 * @brief Register a C-style callback for state changes.
	 * @param onEvent Function pointer called with (side, state).
	 *
	 * @warning The callback is invoked asynchronously (often from a worker/event thread).
	 */
	void AddListenerOnStateChange(void(*onEvent)(ArmSide side, ArmState state));

	/**
	 * @brief Remove a previously registered state-change callback.
	 * @param onEvent Same function pointer passed to AddListenerOnStateChange().
	 */
	void RemoveListenerOnStateChange(void(*onEvent)(ArmSide side, ArmState state));

	/**
	 * @brief Register a C-style callback for movement notifications.
	 * @param onEvent Function pointer called with (side, degreeChange).
	 *
	 * @warning The callback is invoked asynchronously (often from a worker/event thread).
	 */
	void AddListenerOnMovement(void(*onEvent)(ArmSide side, float degreeChange));

	/**
	 * @brief Remove a previously registered movement callback.
	 * @param onEvent Same function pointer passed to AddListenerOnMovement().
	 */
	void RemoveListenerOnMovement(void(*onEvent)(ArmSide side, float degreeChange));

} // namespace ArmEvent