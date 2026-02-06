
/**
 * @example TtsControl/main.cpp
 * @brief Text-to-Speech usage example.
 *
 * Demonstrates:
 * - Initializing TtsControl
 * - Converting text to speech output
 */

#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "TtsControl.h"
#include "SoundControl.h"
#include "Helper.h"


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

	// Initialize Touch Control
	spdlog::info("Initialize and load voice model...");
	if (TtsControl::init(VoiceModel::MODEL_1, "output.wav") < 0)
	{
		spdlog::error("TtsControl init failed");
		return -2;
	}

	// get TtsControl version
	spdlog::info("TtsControl Version:{:.3f}", TtsControl::getVersion());


	// Generate speech sound file from text
	spdlog::info("Produce Text to Speech");
	TtsControl::produce("Hello, this is a text to speech test example.");

	// initialize Sound Control to test sound file
	if (SoundControl::init() < 0)
	{
		spdlog::error("SoundControl init failed");
		return -3;
	}

	// play generated sound file
	SoundControl::play("output.wav", 1);
	// wait for sound to complete
	std::this_thread::sleep_for(std::chrono::seconds(5));

	// Cleanup
	TtsControl::dispose();
	SoundControl::dispose();

	return 0;
}