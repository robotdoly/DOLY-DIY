/**
 * @example ImuControl/main.cpp
 * @brief IMU sensor usage example.
 *
 * Demonstrates:
 * - Initializing ImuControl
 * - Receiving IMU sensor events
 * - Reading orientation and motion data
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "ImuControl.h"
#include "Helper.h"

void onImuUpdate(ImuData data)
{
	spdlog::info("IMU Update - Yaw: {:.2f}, Pitch: {:.2f}, Roll: {:.2f}", data.ypr.yaw, data.ypr.pitch, data.ypr.roll);
}

void onImuGesture(ImuGesture type, GestureDirection from)
{
	spdlog::info("IMU Gesture - Type: {}, Direction: {}", ImuEvent::getGestureStr(type), ImuEvent::getDirectionStr(from));
}

int main()
{
	// Setup spdlog
	spdlog::set_level(spdlog::level::info); // Set as needed
	spdlog::flush_on(spdlog::level::trace); // flush everything

	// *** IMPORTANT *** 
	// Stop doly service if running,
	// otherwise instance of libraries cause conflict	
	if (Helper::stopDolyService() < 0) {
		spdlog::error("Doly service stop failed");
		return -1;
	}

	// For better performance provide actual calibrated IMU offsets or calculate them once after initialization
	// with the help of ImuControl::calculate_offsets() and use them for next initializations.
	// in this example we will use Helper to get previously saved offsets 

	// Read settings
	// one time read is sufficient for the lifetime of the application
	int8_t res = Helper::readSettings();
	if (res < 0)
	{
		spdlog::error("Read settings failed with code: {}", res);
		return -2;
	}

	// Get pre defined IMU offsets
	int16_t gx, gy, gz, ax, ay, az;
	res = Helper::getImuOffsets(gx, gy, gz, ax, ay, az);
	if (res < 0)
	{
		spdlog::error("Get IMU offsets failed with code: {}", res);
		return -3;
	}

	// Get ImuControl version
	spdlog::info("ImuControl Version:{:.3f}", ImuControl::getVersion());

	// Initialize IMU Control with offsets
	// delay 1 second before processing events
	if (ImuControl::init(1, gx, gy, gz, ax, ay, az) < 0)
	{
		spdlog::error("ImuControl init failed");
		return -2;
	}
	
	ImuEvent::AddListenerUpdateEvent(onImuUpdate);
	ImuEvent::AddListenerGestureEvent(onImuGesture);

	// wait for a while before exiting
	std::this_thread::sleep_for(std::chrono::seconds(30));

	// Cleanup
	ImuEvent::RemoveListenerUpdateEvent(onImuUpdate);
	ImuEvent::RemoveListenerGestureEvent(onImuGesture);
	ImuControl::dispose();

	return 0;
}