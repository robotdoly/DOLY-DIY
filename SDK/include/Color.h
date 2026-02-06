#pragma once
#include <string>
#include <vector>
#include <cstdint>

/**
 * @file Color.h
 * @brief Color utilities and predefined color codes used across the Doly SDK.
 *
 * This header provides:
 * - A predefined set of named colors via ColorCode
 * - A simple RGB container (Color)
 * - Helpers to convert between hex strings, names, and codes
 *
 * @ingroup doly_sdk_common
 */

 /**
  * @defgroup doly_sdk_common Common
  * @brief Common/shared SDK types and helper utilities.
  * @{
  */

  /**
   * @brief Predefined color identifiers.
   *
   * These codes map to common RGB values and are used as "semantic colors"
   * across the SDK and examples.
   */
enum class ColorCode : uint8_t
{
	BLACK,      
	WHITE,      
	GRAY,       
	SALMON,     
	RED,        
	DARK_RED,   
	PINK,       
	ORANGE,     
	GOLD,       
	YELLOW,     
	PURPLE,     
	MAGENTA,    
	LIME,       
	GREEN,      
	DARK_GREEN, 
	CYAN,       
	SKY_BLUE,   
	BLUE,       
	DARK_BLUE,  
	BROWN,      
};

/**
 * @brief Simple 8-bit RGB color container and conversion helpers.
 *
 * Values are stored in 8-bit channels (0..255).
 */
struct Color
{
	/** @brief Red channel (0..255). */
	uint8_t r = 0;

	/** @brief Green channel (0..255). */
	uint8_t g = 0;

	/** @brief Blue channel (0..255). */
	uint8_t b = 0;

	/**
	 * @brief Convert this color to a human-readable string.
	 *
	 * Example output: `r[10] g[20] b[30]`
	 *
	 * @return String representation of the color.
	 */
	std::string toString()
	{
		return  "r[" + std::to_string(r) + "] g[ " + std::to_string(g) + "] b[ " + std::to_string(b) + "]";
	}

	/**
	 * @brief Construct a Color from explicit RGB values.
	 * @param r Red channel (0..255).
	 * @param g Green channel (0..255).
	 * @param b Blue channel (0..255).
	 * @return Color instance with the given channels.
	 */
	static Color getColor(uint8_t r, uint8_t g, uint8_t b);

	/**
	 * @brief Convert a hex color string to RGB.
	 *
	 * Typical supported formats are implementation-defined, but commonly include:
	 * - `"#RRGGBB"`
	 * - `"RRGGBB"`
	 *
	 * @param hex Hex string.
	 * @return Parsed Color.
	 *
	 * @note Behavior on invalid input is implementation-defined.
	 */
	static Color hexToRgb(std::string hex);

	/**
	 * @brief Convert a predefined ColorCode to an RGB Color.
	 * @param code Predefined color identifier.
	 * @return RGB Color corresponding to the code.
	 */
	static Color getColor(ColorCode code);

	/**
	 * @brief Convert a ColorCode to the closest LED-safe color tone.
	 *
	 * Some LED hardware has limited color gamut and brightness curves.
	 * This function returns a color tone tuned for the LEDs.
	 *
	 * @param code Predefined color identifier.
	 * @return RGB Color best suited for LED output.
	 */
	static Color getLEDColor(ColorCode code);

	/**
	 * @brief Get a human-readable name for a color code.
	 * @param code Predefined color identifier.
	 * @return Color name (e.g., "COLOR_BLACK").
	 */
	static std::string getColorName(ColorCode code);

private:
	/**
	 * @brief Utility: split a string into equal chunks.
	 *
	 * Used internally by parsing helpers.
	 *
	 * @param str Source string.
	 * @param splitLength Chunk length.
	 * @return Vector of chunks.
	 */
	static std::vector<std::string> splitWithCharacters(const std::string& str, int splitLength);
};

/** @} */ // end of group doly_sdk_common