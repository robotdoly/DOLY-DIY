#pragma once
#include <cstdint>

/**
 * @file Helper.h
 * @brief Shared helper utilities and common types used across the Doly SDK.
 *
 * This header is intentionally lightweight and is used by multiple modules
 * (e.g., DriveControl) and most examples/tests.
 *
 * @ingroup doly_sdk_common
 */

 /**
  * @defgroup doly_sdk_common Common
  * @brief Common/shared SDK types and helper utilities.
  * @{
  */

  /**
   * @brief 3D pose-like position with heading and integer XY coordinates.
   *
   * This type is typically used by DriveControl to represent the robot's estimated
   * position and heading. The exact coordinate frame and units depend on the
   * DriveControl implementation and calibration.
   */
struct Position
{
    /**
     * @brief Heading angle in degrees.
     *
     * @note Range and sign convention are implementation-defined. Commonly 0..360.
     */
    float head;

    /**
     * @brief X coordinate in integer units.
     *
     * @note Units depend on the implementation (e.g., mm, ticks, or map units).
     */
    int x;

    /**
     * @brief Y coordinate in integer units.
     *
     * @note Units depend on the implementation (e.g., mm, ticks, or map units).
     */
    int y;
};

/**
 * @brief 2D position using float coordinates.
 *
 * Often used for intermediate calculations or UI mapping.
 */
struct Position2F
{
    /** @brief X coordinate. Units are implementation-defined. */
    float x;

    /** @brief Y coordinate. Units are implementation-defined. */
    float y;
};

namespace Helper
{
    /**
     * @brief Read default settings from the platform settings file.
     *
     * This is commonly used to load calibration/config values that the SDK
     * uses at runtime.
     *
     * @return Status code:
     * - >= 0 : success
     * - -1   : settings file not found
     * - -2   : XML open or parse error
     */
    int8_t readSettings();

    /**
     * @brief Retrieve IMU calibration offsets (gyro/accel).
     *
     * Reads previously stored IMU offset values. These offsets are typically passed
     * into DriveControl::init(...) to start with calibrated parameters.
     *
     * @param[out] gx Gyro X offset.
     * @param[out] gy Gyro Y offset.
     * @param[out] gz Gyro Z offset.
     * @param[out] ax Accel X offset.
     * @param[out] ay Accel Y offset.
     * @param[out] az Accel Z offset.
     *
     * @return Status code:
     * - 0  : success
     * - -1 : failed to read offsets
     */
    int8_t getImuOffsets(int16_t& gx, int16_t& gy, int16_t& gz,
        int16_t& ax, int16_t& ay, int16_t& az);

    /**
     * @brief Stop the background Doly service (if running).
     *
     * Some applications/tests may need exclusive access to hardware resources
     * owned by the service. This helper provides a unified method to request
     * stopping it.
     *
     * @return Status code:
     * - 0  : success
     * - 1  : service not active
     * - -1 : error while stopping service
     *
     * @warning Stopping the service may affect other running applications.
     */
    int8_t stopDolyService();

} // namespace Helper

/** @} */ // end of group doly_sdk_common