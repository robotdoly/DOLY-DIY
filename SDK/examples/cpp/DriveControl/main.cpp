/**
 * @example DriveControl/main.cpp
 * @brief DriveControl basic usage example.
 *
 * Demonstrates:
 * - Initializing the DriveControl module
 * - Registering DriveEventListener
 * - Sending basic drive and motion commands
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include <DriveControl.h>
#include <DriveEvent.h>

void onDriveComplete(uint16_t id) {
	spdlog::info("Drive complete id={}", id);
}

void onDriveError(std::uint16_t id, DriveMotorSide side, DriveErrorType type) {
	spdlog::error("Drive error id={} side={} type={}",
		id, (int)side, (int)type);
}

void onDriveStateChange(DriveType driveType, DriveState state) {
	spdlog::info("Drive state type={} state={}", (int)driveType, (int)state);
}

int main() {
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

	// get DriveControl version
	spdlog::info("DriveControl Version:{:.3f}", DriveControl::getVersion());

	// Register event listeners
	DriveEvent::AddListenerOnComplete(onDriveComplete);
	DriveEvent::AddListenerOnError(onDriveError);
	DriveEvent::AddListenerOnStateChange(onDriveStateChange);

	// Initialize DriveControl with zero IMU offsets (example)
	// For better performance provide actual calibrated IMU offsets.	
	// Check HelperExample for reading offsets from settings
	if (DriveControl::init() != 0) {
		spdlog::error("DriveControl init failed");
		return -2;
	}

	uint8_t speed = 50; // example speed % (0..100)

	// Example 1: go 100mm forward, brake at end (non-blocking)
	DriveControl::goDistance(1, 100, speed, true, true);

	// Example 2: Rotate 45 degrees counterclockwise on wheel, with brake at end (non-blocking)	
	//DriveControl::goRotate(2, -45, false, speed, true, true);

	// Example 3: Go to X= -100mm, Y= 200mm, approaching forward with accelaration, without brake at end (non-blocking)
	// DriveControl::goXY(1, -100, 200, speed, true, false, 50);

	// Example 4: Free drive left and right wheels forward at specified speed for 2 seconds (non-blocking)
	// DriveControl::freeDrive(speed, false, true); // Low-level drive right 
	// DriveControl::freeDrive(speed, true, true);  // Low-level drive left 
	// drive for 2 seconds
	// std::this_thread::sleep_for(std::chrono::seconds(2)); 
	// Stop driving wheels
	// DriveControl::freeDrive(0, false, true);  
	// DriveControl::freeDrive(0, true, true);  

	// Wait until done (simple polling example)
	while (DriveControl::getState() == DriveState::RUNNING) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	// Get final position
	// Position calculated based on wheel odometry and IMU data,
	// for better accuracy use predefined offsets in DriveControl::init()
	auto pos = DriveControl::getPosition();
	spdlog::info("Robot pos x={} y={} head={}", pos.x, pos.y, pos.head);

	// Cleanup
	DriveControl::dispose(true); // dispose IMU as well
	DriveEvent::RemoveListenerOnComplete(onDriveComplete);
	DriveEvent::RemoveListenerOnError(onDriveError);
	DriveEvent::RemoveListenerOnStateChange(onDriveStateChange);

	return 0;
}
