#pragma once
#include <vector>
#include <stdint.h>
#include "GpioPin.h"

/**
 * @file EdgeEventListener.h
 * @brief Public API types and listener interface for EdgeControl.
 *
 * This header defines:
 * - public enums (GapDirection, SensorId)
 * - IrSensor POD type
 * - EdgeEventListener callback interface
 *
 * @ingroup doly_edgecontrol
 */

 /**
  * @defgroup doly_edgecontrol EdgeControl
  * @brief Doly edge/gap detection SDK (C++) (IR sensors).
  *
  * EdgeControl is exposed as a singleton-style API (namespace functions).
  * Events are delivered asynchronously through EdgeEvent / EdgeEventListener.
  * @{
  */

  /**
   * @brief Direction of a detected gap or edge relative to the robot.
   *
   * These values indicate where a gap (e.g. missing floor)
   * was detected by the IR sensors.
   */
enum class GapDirection : uint8_t
{
	FRONT,        /**< Gap detected at the front. */
	FRONT_LEFT,   /**< Gap detected at the front-left. */
	FRONT_RIGHT,  /**< Gap detected at the front-right. */
	BACK,         /**< Gap detected at the rear. */
	BACK_LEFT,    /**< Gap detected at the rear-left. */
	BACK_RIGHT,   /**< Gap detected at the rear-right. */
	LEFT,         /**< Gap detected on the left side. */
	RIGHT,        /**< Gap detected on the right side. */
	CROSS_LEFT,   /**< Cross pattern: front-left, back-right. */
	CROSS_RIGHT,  /**< Cross pattern: front-right, back-left. */
	ALL,          /**< All sensors detect a gap. */
};

/**
 * @brief Identifiers for the IR sensors.
 */
enum class SensorId : uint8_t
{
	BACK_LEFT,   /**< Rear-left IR sensor. */
	BACK_RIGHT,  /**< Rear-right IR sensor. */
	FRONT_LEFT,  /**< Front-left IR sensor. */
	FRONT_RIGHT, /**< Front-right IR sensor. */
};

/**
 * @brief Simple POD representing the state of an individual IR sensor.
 *
 * @note `GpioState` is defined in `GpioPin.h` and represents
 *       high/low states for a IR sensor.
 */
struct IrSensor
{
	SensorId id;     /**< Which sensor this reading came from. */
	GpioState state; /**< Current GPIO state of the sensor. */
};

/**
 * @brief Interface for receiving edge and gap detection events from edge controller.
 *
 * Implement this interface and register it using EdgeEvent::AddListener().
 *
 * @note Default implementations are no-ops, so you may override only what you need.
 * @warning Callbacks are typically invoked from an internal worker/event thread.
 *          Keep handlers fast and avoid blocking.
 * @warning The listener object must remain valid until it is removed via
 *          EdgeEvent::RemoveListener().
 */
class EdgeEventListener
{
public:
	/**
	 * @brief Virtual destructor for proper cleanup in derived classes.
	 */
	virtual ~EdgeEventListener() = default;

	/**
	 * @brief Called when the set of IR sensor states changes.
	 *
	 * @param sensors Vector of `IrSensor` entries describing the sensors
	 *                that changed state. The container may include one or multiple sensors.
	 */
	virtual void onEdgeChange(std::vector<IrSensor> sensors);

	/**
	 * @brief Called when a gap (e.g. drop-off, missing surface) is detected and categorized.
	 *
	 * @param gap_type Coarse direction/category of the detected gap.
	 */
	virtual void onGapDetect(GapDirection gap_type);
};

/** @} */ // end of group doly_edgecontrol