#pragma once
#include <cstdint>

/**
 * @file GpioPin.h
 * @brief Common GPIO types used across the Doly SDK.
 *
 * This header contains shared enums that describe GPIO configuration
 * and digital state values used by GPIO-related modules.
 *
 * @ingroup doly_sdk_common
 */

 /**
  * @defgroup doly_sdk_common Common
  * @brief Common/shared SDK types and helper utilities.
  * @{
  */

  /**
   * @enum GpioType
   * @brief Describes the configured role of a GPIO pin.
   */
enum class GpioType : uint8_t
{
	/** Pin configured as a digital input. */
	INPUT,

	/** Pin configured as a digital output. */
	OUTPUT,

	/** Pin configured for hardware PWM output. */
	PWM,

	/** Pin configured as an analog input (ADC). */
	AIN
};

/**
 * @enum GpioState
 * @brief Represents the digital logic level of a GPIO pin.
 *
 * - LOW  -> 0
 * - HIGH -> 1
 */
enum class GpioState : uint8_t
{
	/** Logical low (0V). */
	LOW,

	/** Logical high (VCC). */
	HIGH,
};

/** @} */ // end of group doly_sdk_common