#pragma once
#include "EdgeEventListener.h"

/**
 * @file EdgeEvent.h
 * @brief Event registration functions for EdgeControl.
 *
 * This header exposes listener registration/removal for edge-related events.
 * It supports:
 * - Object listeners (EdgeEventListener)
 * - Optional "static callback" listeners (function pointers)
 *
 * @ingroup doly_edgecontrol
 */

namespace EdgeEvent
{
	/**
	 * @brief Register an EdgeEventListener instance.
	 *
	 * @param observer Listener object pointer.
	 * @param priority If true, the listener may be inserted with higher priority
	 *                 (implementation-defined ordering).
	 *
	 * @warning The observer must remain valid until removed via RemoveListener().
	 */
	void AddListener(EdgeEventListener* observer, bool priority = false);

	/**
	 * @brief Unregister an EdgeEventListener instance.
	 *
	 * @param observer Listener object pointer previously registered.
	 */
	void RemoveListener(EdgeEventListener* observer);

	/**
	 * @brief Register a function callback for sensor state changes.
	 *
	 * @param onChange Function pointer invoked on changes.
	 *
	 * @warning The callback is typically invoked from an internal worker/event thread.
	 */
	void AddListenerOnChange(void(*onChange)(std::vector<IrSensor> sensors));

	/**
	 * @brief Unregister a function callback for sensor state changes.
	 *
	 * @param onChange Function pointer previously registered.
	 */
	void RemoveListenerOnChange(void(*onChange)(std::vector<IrSensor> sensors));

	/**
	 * @brief Register a function callback for gap detection events.
	 *
	 * @param onChange Function pointer invoked with the detected gap direction.
	 *
	 * @warning The callback is typically invoked from an internal worker/event thread.
	 */
	void AddListenerOnGapDetect(void(*onChange)(GapDirection gap_type));

	/**
	 * @brief Unregister a function callback for gap detection events.
	 *
	 * @param onChange Function pointer previously registered.
	 */
	void RemoveListenerOnGapDetect(void(*onChange)(GapDirection gap_type));

} // namespace EdgeEvent