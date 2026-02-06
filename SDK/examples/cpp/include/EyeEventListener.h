#pragma once
#include <stdint.h>

/**
 * @file EyeEventListener.h
 * @brief Public listener interface for EyeControl events.
 *
 * EyeControl exposes a singleton-style API (namespace functions).
 * State changes are reported asynchronously through EyeEvent / EyeEventListener.
 *
 * @ingroup doly_eyecontrol
 */

 /**
  * @defgroup doly_eyecontrol EyeControl
  * @brief Doly eye display/animation SDK (C++).
  *
  * EyeControl drives the robot's LCD eyes, including iris/lids/background and
  * named animations (see EyeExpressions).
  * Events are delivered asynchronously through EyeEvent / EyeEventListener.
  * @{
  */

  /**
   * @brief Interface for receiving EyeControl events.
   *
   * Implement this interface and register it using EyeEvent::AddListener().
   *
   * @note Default implementations are no-ops, so you may override only what you need.
   * @warning Callbacks are typically invoked from an internal worker/event thread.
   *          Keep handlers fast and avoid blocking.
   * @warning The listener object must remain valid until it is removed via
   *          EyeEvent::RemoveListener().
   */
class EyeEventListener
{
public:
	/** @brief Virtual destructor for proper cleanup in derived classes. */
	virtual ~EyeEventListener() = default;

	/**
	 * @brief Called when an eye action/animation starts.
	 * @param id User-provided animation/action id (forwarded from EyeControl).
	 */
	virtual void onEyeStart(uint16_t id);

	/**
	 * @brief Called when an eye action/animation completes.
	 * @param id User-provided animation/action id (forwarded from EyeControl).
	 */
	virtual void onEyeComplete(uint16_t id);

	/**
	 * @brief Called when an eye action/animation is aborted/stopped.
	 * @param id User-provided animation/action id (forwarded from EyeControl).
	 */
	virtual void onEyeAbort(uint16_t id);
};

/** @} */ // end of group doly_eyecontrol