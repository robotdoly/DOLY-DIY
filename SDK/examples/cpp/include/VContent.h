#pragma once
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <vector>

/**
 * @file VContent.h
 * @brief Visual content container for Doly displays.
 *
 * VContent represents image-based visual content that can be rendered on Doly's displays.
 * It supports single-frame images as well as multi-frame sequences (animations).
 *
 * This class is part of the Doly *common* module and is shared across multiple subsystems
 * (e.g., Eye/LCD rendering, animations).
 *
 * Design notes:
 * - Frames are stored as raw byte buffers
 * - All frames in a sequence must have identical dimensions
 *
 * @defgroup doly_common_vcontent VContent
 * @brief Common visual content container used across Doly subsystems.
 * @{
 */

 /**
  * @brief Container class for visual (image/animation) content.
  */
class VContent
{
public:
	VContent() = default;
	~VContent() = default;

	/**
	 * @brief Check whether the visual content is loaded and ready.
	 * @return true if content was loaded successfully; false otherwise.
	 */
	bool isReady();

	/**
	 * @brief Image frame buffers.
	 *
	 * Each frame is stored as a vector of bytes.
	 * - Single-image content uses frames[0]
	 * - Multi-frame content (animations) are stored sequentially:
	 *   frames[0], frames[1], ...
	 */
	std::vector<std::vector<uint8_t>> frames;

	/** @brief Selected frame index on load. */
	uint16_t active_frame_id = 0;

	/** @brief Total number of frames in the sequence. */
	uint16_t ft = 0;

	/** @brief Frame width in pixels. */
	uint16_t width = 0;

	/** @brief Frame height in pixels. */
	uint16_t height = 0;

	/** @brief Source path of the image/animation. */
	std::string path;

	/** @brief True if the source image has an alpha channel. */
	bool alpha;

	/** @brief True if image color depth is 12-bit. */
	bool color12Bit;

	/**
	 * @brief Frame rate divider.
	 *
	 * Example:
	 * - ratio = 2 means play at (base_fps / 2)
	 */
	uint8_t ratio = 1;

	/**
	 * @brief Loop count for animations.
	 *
	 * - loop = 0 : loop forever
	 * - loop = 1 : play once
	 */
	uint16_t loop = 0;

	/**
	 * @brief Load a PNG image and create a VContent instance.
	 *
	 * Supported formats:
	 * - 8-bit RGB
	 * - 8-bit RGBA
	 * - 16-bit RGB
	 * - 16-bit RGBA
	 *
	 * @param path Path to the image file.
	 * @param isRGBA True if the image contains an alpha channel.
	 * @param set12Bit If true, convert image to 12-bit color depth.
	 *
	 * @return VContent instance containing the loaded image data.
	 *
	 * @note Color depth conversion does not change buffer size; data remains
	 * 8-bit per channel.
	 */
	static VContent getImage(std::string path, bool isRGBA, bool set12Bit);

private:
	/** @brief Internal loaded flag. */
	bool loaded = false;
};

/** @} */ // end of group doly_common_vcontent
