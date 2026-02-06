/**
 * @example SoundControl/main.cpp
 * @brief Sound playback and control example.
 *
 * Demonstrates:
 * - Initializing SoundControl
 * - Playing sound files
 * - Handling sound-related events
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "SoundControl.h"
#include "Helper.h"

void onSoundBegin(uint16_t id, float volume)
{
	spdlog::info("Sound begin id:{}, volume:{:.2f}", id, volume);
}

void onSoundComplete(uint16_t id)
{
	spdlog::info("Sound complete id:{}", id);
}

void onSoundAbort(uint16_t id)
{
	spdlog::info("Sound aborted id:{}", id);
}

void onSoundError(uint16_t id)
{
	spdlog::error("Sound error id:{}", id);
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

	// Initialize Sound Control
	if (SoundControl::init() < 0)
	{
		spdlog::error("SoundControl init failed");
		return -2;
	}

	// get SoundControl version
	spdlog::info("SoundControl Version:{:.3f}", SoundControl::getVersion());

	// Add event listeners
	SoundEvent::AddListenerOnBegin(onSoundBegin);
	SoundEvent::AddListenerOnComplete(onSoundComplete);
	SoundEvent::AddListenerOnAbort(onSoundAbort);
	SoundEvent::AddListenerOnError(onSoundError);

	// Set volume
	SoundControl::setVolume(80); // set volume to 80%

	// Play sound
	int ret = SoundControl::play("../sound_test.wav", 1);
	if (ret < 0)
		spdlog::error("Play failed, error code:{}", ret);

	// Wait for sound to complete
	while (ret == 0 && SoundControl::getState() != SoundState::STOP)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	// Cleanup
	SoundEvent::RemoveListenerOnBegin(onSoundBegin);
	SoundEvent::RemoveListenerOnComplete(onSoundComplete);
	SoundEvent::RemoveListenerOnAbort(onSoundAbort);
	SoundEvent::RemoveListenerOnError(onSoundError);

	SoundControl::dispose();

	return 0;
}