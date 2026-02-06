/**
 * @example ArmControl/main.cpp
 * @brief ArmControl basic usage example.
 *
 * Demonstrates:
 * - Initializing the ArmControl module
 * - Registering ArmEventListener
 * - Sending basic arm movement commands
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include <ArmControl.h>
#include <ArmEvent.h>
#include <Helper.h>

void onArmComplete(uint16_t id, ArmSide side)
{
	spdlog::info("Arm complete id={} side={}", id, (int)side);
}

void onArmError(uint16_t id, ArmSide side, ArmErrorType errorType)
{
	spdlog::error("Arm error id={} side={} type={}",
		id, (int)side, (int)errorType);
}

void onArmStateChange(ArmSide side, ArmState state)
{
	spdlog::info("Arm state side={} state={}", (int)side, (int)state);
}

void onArmMovement(ArmSide side, float degreeChange)
{
	spdlog::info("Arm movement side={} degreeChange={}", (int)side, degreeChange);
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

	// Read settings for servo calibration data, otherwise arm control won't work.
	// one time read is sufficient for the lifetime of the application
	int8_t res = Helper::readSettings();
	if (res < 0)
	{
		spdlog::error("Read settings failed with code: {}", res);
		return -1;
	}

	// get ArmControl version
	spdlog::info("ArmControl Version:{:.3f}", ArmControl::getVersion());

	// Register event listeners if needed
	ArmEvent::AddListenerOnComplete(onArmComplete);
	ArmEvent::AddListenerOnError(onArmError);
	ArmEvent::AddListenerOnStateChange(onArmStateChange);
	ArmEvent::AddListenerOnMovement(onArmMovement);

	// Initialize ArmControl
	if (ArmControl::init() < 0)
	{
		spdlog::error("ArmControl init failed");
		return -2;
	}

	// Set arm angle (non-blocking)
	ArmControl::setAngle(1, ArmSide::BOTH, 50, 90, false);

	// Wait until done (simple polling example)
	while (ArmControl::getState(ArmSide::BOTH) != ArmState::COMPLETED) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	// Get position of arms
	// Position calculated based on servo potantiometer feedback,
	// and may not be accurate due to mechanical slippage etc.
	auto arms = ArmControl::getCurrentAngle(ArmSide::BOTH);
	spdlog::info("Estimated Arm Positions:");
	for (const auto& a : arms) {
		spdlog::info("  Side:{} Angle:{}", (int)a.side, a.angle);
	}

	// Cleanup
	ArmEvent::RemoveListenerOnComplete(onArmComplete);
	ArmEvent::RemoveListenerOnError(onArmError);
	ArmEvent::RemoveListenerOnStateChange(onArmStateChange);
	ArmEvent::RemoveListenerOnMovement(onArmMovement);
	ArmControl::dispose();	

	return 0;
}