#pragma once
#include <stdint.h>
#include <string>

/**
 * @file TtsControl.h
 * @brief Public API for Doly TTS (Text-to-Speech) control.
 *
 * This header provides a simple interface for initializing a TTS engine and producing
 * speech audio from text. The implementation is a wrapper around the Piper TTS library.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - init() must be called before produce()
 * - Output audio is written to a file (see init() / output_path)
 *
 * @defgroup doly_ttscontrol TtsControl
 * @brief Doly TTS (Text-to-Speech) control API.
 * @{
 */

 // Thanks to piper TTS library, this is a wrapper for it.

 /**
  * @brief Available voice model selections.
  *
  * Model-to-file mapping is implementation-defined. init() returns an error if the
  * required model files are missing.
  */
enum class VoiceModel :uint8_t
{
	MODEL_1,
	MODEL_2,
	MODEL_3,
};

namespace TtsControl
{
	/**
	 * @brief Initialize the TTS system.
	 *
	 * @param model Voice model to use.
	 * @param output_path Optional output directory/path used by the implementation.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already initialized
	 * - -1 : model file missing
	 * - -2 : model config file missing
	 * 
	 * @note Model loading is a time-consuming and CPU-intensive operation; it is a blocking function.
	 */
	int8_t init(VoiceModel model, std::string output_path = "");

	/**
	 * @brief Dispose the TTS system and release resources.
	 * @return Status code (implementation-defined).
	 */
	int8_t dispose();

	/**
	 * @brief Produce a speech audio output from text.
	 *
	 * @param text Input text to synthesize.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : TTS not active (init() not called or failed)
	 * - -2 : data processing error
	 *
	 * @note This operation blocking the thread.
	 */
	int8_t produce(std::string text);

	/**
	 * @brief Get the TTS subsystem/library version.
	 * @return Version number.
	 */
	float getVersion();

};

/** @} */ // end of group doly_ttscontrol
