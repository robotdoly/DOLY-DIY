#pragma once
#include "SoundEvent.h"

/**
 * @file SoundControl.h
 * @brief Public API for Doly sound playback control.
 *
 * This API provides non-blocking sound playback using an internal worker thread.
 * Playback lifecycle events (begin/complete/abort/error) can be observed via SoundEvent.
 *
 * Supported audio format (as required by the implementation):
 * - File: WAV
 * - Sample format: S16
 * - Channels: 2 (stereo)
 * - Sample rate: 48000 Hz
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - init() must be called before play()
 * - play() is non-blocking (handled in another thread)
 *
 * @ingroup doly_soundcontrol
 */

 /**
  * @brief Sound activity state.
  */
enum class SoundState :uint8_t
{
	SET,
	STOP,
	PLAY
};

namespace SoundControl
{
	/**
	 * @brief Initialize sound control.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already running
	 * - -1 : loading failed
	 */
	int8_t init();

	/**
	 * @brief Dispose/stop the sound subsystem and release resources.
	 *
	 * @return Status code:
	 * - 0 : success
	 * - 1 : not initialized
	 */
	int8_t dispose();

	/**
	 * @brief Start playing a sound file (non-blocking).
	 *
	 * @param file_name Full path of the sound file.
	 * @param id User-defined sound identifier forwarded to SoundEvent callbacks.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : sound control not initialized
	 * - -2 : file not found
	 *
	 * @note Playback happens asynchronously; use SoundEvent callbacks to track progress.
	 */
	int8_t play(std::string file_name, uint16_t id);

	/**
	 * @brief Abort the currently playing sound (if any).
	 */
	void Abort();

	/**
	 * @brief Get current sound playback state.
	 * @return Current SoundState.
	 */
	SoundState getState();

	/**
	 * @brief Check whether the sound subsystem is active (initialized).
	 * @return true if initialized; false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Set playback volume as a percentage.
	 *
	 * @param volume Volume percentage (0..100).
	 * @return 0 on success; negative value on failure.
	 */
	int8_t setVolume(uint8_t volume);

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();

};
