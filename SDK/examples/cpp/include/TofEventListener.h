#pragma once
#include <stdint.h>

/**
 * @file TofEventListener.h
 * @brief Listener interface and data types for ToF (Time of Flight) sensor events.
 *
 * This header defines:
 * - ToF error codes (TofError)
 * - Sensor side selection (TofSide)
 * - Gesture types and structures (TofGestureType, TofGesture)
 * - Sensor data structure (TofData)
 * - The TofEventListener callback interface used by TofEvent
 *
 * Threading notes:
 * - Callbacks may be invoked from an internal worker/event thread.
 * - Keep listener objects alive while they are registered (see TofEvent::AddListener()).
 *
 * Reference:
 * - STMicroelectronics VL6180 API user manual (UM2760) for error code meanings.
 *
 * @defgroup doly_tofcontrol TofControl
 * @brief Doly ToF (Time of Flight) control and event API.
 * @{
 */

 /**
  * @brief ToF error codes reported by the sensor/driver.
  *
  * Values are based on the VL6180 API/user manual (UM2760).
  */
enum class TofError : uint8_t
{
	NO_ERROR = 0,
	VCSEL_Continuity_Test = 1,
	VCSEL_Watchdog_Test = 2,
	VCSEL_Watchdog = 3,
	PLL1_Lock = 4,
	PLL2_Lock = 5,
	Early_Convergence_Estimate = 6,
	Max_Convergence = 7,
	No_Target_Ignore = 8,
	Max_Signal_To_Noise_Ratio = 11,
	Raw_Ranging_Algo_Underflow = 12,
	Raw_Ranging_Algo_Overflow = 13,
	Ranging_Algo_Underflow = 14,
	Ranging_Algo_Overflow = 15,
	Filtered_by_post_processing = 16,
	DataNotReady = 18,
};

/**
 * @brief Which ToF sensor is addressed.
 */
enum class TofSide :uint8_t
{
	LEFT,
	RIGHT
};

/**
 * @brief High-level gesture types derived from ToF continuous readings.
 */
enum class TofGestureType :uint8_t
{
	UNDEFINED,
	OBJECT_COMING,
	OBJECT_GOING,
	SCRUBING,
	TO_LEFT,
	TO_RIGHT
};

/**
 * @brief Gesture description for a ToF sensor.
 */
struct TofGesture
{
	/** Gesture type. */
	TofGestureType type;
	/** Associated range in millimeters. */
	int range_mm = 0;
};

/**
 * @brief Latest ToF sample snapshot for one sensor.
 */
struct TofData
{
	/** Update timestamp in milliseconds. */
	int64_t update_ms = 0;
	/** Measured range in millimeters. */
	int range_mm = 0;
	/** Error status for this sample. */
	TofError error = TofError::NO_ERROR;
	/** Sensor side for this data. */
	TofSide side;
};

/**
 * @brief Observer interface for receiving ToF gesture and threshold events.
 *
 * Register an implementation using TofEvent::AddListener().
 *
 * @warning Do not destroy a listener object while it is registered.
 */
class TofEventListener
{
public:
	virtual ~TofEventListener() = default;

	/**
	 * @brief Called when a proximity gesture is detected.
	 * @param left Gesture result for the left sensor.
	 * @param right Gesture result for the right sensor.
	 */
	virtual void onProximityGesture(TofGesture left, TofGesture right);

	/**
	 * @brief Called when proximity threshold is reached (if enabled).
	 * @param left Latest left sensor data.
	 * @param right Latest right sensor data.
	 */
	virtual void onProximityThreshold(TofData left, TofData right) {}
};

/** @} */ // end of group doly_tofcontrol
