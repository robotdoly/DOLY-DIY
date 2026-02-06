#pragma once
#include <stdint.h>

/**
 * @file ImuEventListener.h
 * @brief Listener interface and data types for IMU (Inertial Measurement Unit) events.
 *
 * This header defines:
 * - Gesture enums (ImuGesture, GestureDirection)
 * - IMU data structs (VectorFloat, YawPitchRoll, ImuData)
 * - The ImuEventListener callback interface used by ImuEvent
 *
 * Threading notes:
 * - Callbacks invoked from an internal worker/event thread.
 * - Keep listener objects alive while they are registered (see ImuEvent::AddListener()).
 *
 * @ingroup doly_imucontrol
 */

 /** @defgroup doly_imucontrol ImuControl
  *  @brief Doly IMU (Inertial Measurement Unit) control and event API.
  *  @{
  */

  /**
   * @brief High-level gesture types detected from IMU motion analysis.
   */
enum class ImuGesture :uint8_t
{
	/** No gesture / unknown state. */
	UNDEFINED,
	/** Minor movement detected. */
	MOVE,
	/** Long-distance shake detected. */
	LONG_SHAKE,
	/** Short-distance shake detected. */
	SHORT_SHAKE,
	/** Vibration detected. */
	VIBRATE,
	/** Strong/continuous vibration detected. */
	VIBRATE_EXTREME,
	/** Light shock/impact detected. */
	SHOCK_LIGHT,
	/** Medium shock/impact detected. */
	SHOCK_MEDIUM,
	/** Hard shock/impact detected. */
	SHOCK_HARD,
	/** Extreme shock/impact detected. */
	SHOCK_EXTREME,
};

/**
 * @brief Direction associated with a detected gesture (where the motion came from).
 */
enum class GestureDirection : uint8_t
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	FRONT,
	BACK
};

/**
 * @brief 3D float vector (x,y,z).
 */
struct VectorFloat
{
	float x;
	float y;
	float z;
};

/**
 * @brief Yaw / Pitch / Roll angles (degrees).
 */
struct YawPitchRoll
{
	float yaw;
	float pitch;
	float roll;
};

/**
 * @brief Latest IMU reading snapshot returned by the IMU subsystem.
 *
 * Contains computed orientation (YPR), linear acceleration, and temperature.
 */
struct ImuData
{
	/** Yaw/Pitch/Roll angles. */
	YawPitchRoll ypr;
	/** Linear acceleration vector (implementation-defined units). */
	VectorFloat linear_accel;
	/** Temperature in degrees Celsius. */
	float temperature;
};

/**
 * @brief Observer interface for receiving IMU events.
 *
 * Register an implementation using ImuEvent::AddListener().
 *
 * @warning Do not destroy a listener object while it is registered.
 */
class ImuEventListener
{
public:
	virtual ~ImuEventListener() = default;

	/**
	 * @brief Called when a new IMU sample/update is available.
	 * @param data Latest IMU snapshot.
	 */
	virtual void onImuUpdate(ImuData data);

	/**
	 * @brief Called when a gesture is detected.
	 * @param type Detected gesture type.
	 * @param from Direction associated with the gesture.
	 */
	virtual void onImuGesture(ImuGesture type, GestureDirection from);
};
/** @} */ // end of group doly_imucontrol
