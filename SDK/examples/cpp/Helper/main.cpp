/**
 * @example Helper/main.cpp
 * @brief Helper utilities usage example.
 *
 * Demonstrates:
 * - Using common helper utilities
 * - Working with shared SDK helper types
 */

#include <Helper.h>
#include <spdlog/spdlog.h>

int main()
{
	// Setup spdlog
	spdlog::set_level(spdlog::level::info); // Set as needed
	spdlog::flush_on(spdlog::level::trace); // flush everything

	// Read settings
	// one time read is sufficient for the lifetime of the application
	int8_t res = Helper::readSettings();
	if (res < 0)
	{
		spdlog::error("Read settings failed with code: {}", res);
		return -1;
	}

	// Get IMU offsets
	int16_t gx, gy, gz, ax, ay, az;
	res = Helper::getImuOffsets(gx, gy, gz, ax, ay, az);
	if (res < 0)
	{
		spdlog::error("Get IMU offsets failed with code: {}", res);
		return -1;
	}
	spdlog::info("IMU Offsets - Gx:{} Gy:{} Gz:{} Ax:{} Ay:{} Az:{}", gx, gy, gz, ax, ay, az);
	// end of IMU offsets

	return 0;
}