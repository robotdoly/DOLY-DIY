#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <Color.h>
#include <EyeEvent.h>
#include <VContent.h>

/**
 * @file EyeControl.h
 * @brief Public API for Doly eye display control and animations.
 *
 * Design notes:
 * - Singleton-style control (namespace API; no instances)
 * - Functions require init() before use
 * - Animation lifecycle is reported asynchronously through EyeEvent/EyeEventListener
 *
 * Threading notes:
 * - setAnimation() is non-blocking; animations run on an internal worker thread
 * - Event callbacks are typically invoked from an internal worker/event thread
 *
 * @ingroup doly_eyecontrol
 */

 /**
  * @brief Which eye(s) a command targets.
  */
enum class EyeSide :uint8_t
{
	BOTH,  /**< Apply to both eyes. */
	LEFT,  /**< Apply to left eye. */
	RIGHT  /**< Apply to right eye. */
};

/**
 * @brief Built-in iris style presets.
 */
enum class IrisShape :uint8_t
{
	CLASSIC, /**< Classic iris style. */
	MODERN,  /**< Modern iris style. */
	SPACE,   /**< Space-themed iris style. */
	ORBIT,   /**< Orbit-themed iris style. */
	GLOW,    /**< Glow iris style. */
	DIGI,    /**< Digital iris style. */
};

namespace EyeControl
{
	/**
	 * @brief Initialize the eye subsystem (LCD + eye assets).
	 *
	 * This must be called once before other control functions.
	 *
	 * @param eye_color Default iris color (see Color.h).
	 * @param bg_color Default background color (see Color.h).
	 *
	 * @return Status code:
	 * - 0  : success
	 * - 1  : already initialized
	 * - -1 : loadEyeFiles failed
	 * - -2 : LCD init failed
	 */
	int8_t init(ColorCode eye_color, ColorCode bg_color);

	/**
	 * @brief Abort/terminate the active animation.
	 *
	 * This is intended as a stop/emergency action for ongoing animations.
	 */
	void Abort();

	/**
	 * @brief Check whether the subsystem is active (initialized and running).
	 * @return true if initialized, false otherwise.
	 */
	bool isActive();

	/**
	 * @brief Check whether an animation is currently running.
	 * @return true if an animation continues, false otherwise.
	 */
	bool isAnimating();

	/**
	 * @brief Set both iris and background to built-in presets.
	 *
	 * @param shape Built-in iris shape preset.
	 * @param iris_color Iris color.
	 * @param bg_color Background color.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : getIris() failed, out of array
	 * - -2 : getBackground() failed, out of array
	 */
	int8_t setEyes(IrisShape shape, ColorCode iris_color, ColorCode bg_color);

	/**
	 * @brief Set iris preset for a given side.
	 *
	 * @param shape Iris preset to apply.
	 * @param color Iris color.
	 * @param side Target eye side.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : out of array, check color
	 * - -2 : out of array, check shape
	 */
	int8_t setIris(IrisShape shape, ColorCode color, EyeSide side);

	/**
	 * @brief Set iris image content for a given side.
	 *
	 * @param eye_image Iris image content.
	 * @param side Target eye side.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : null image pointer
	 */
	int8_t setIris(VContent* eye_image, EyeSide side);

	/**
	 * @brief Set eyelid image content for a given side.
	 *
	 * @param lid_image Lid image content.
	 * @param isTop True for top lid, false for bottom lid.
	 * @param side Target eye side.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : null image pointer
	 */
	int8_t setLid(VContent* lid_image, bool isTop, EyeSide side);

	/**
	 * @brief Set background color (solid).
	 * @param color Background color (RGB).
	 * @return Status code:
	 * - 0  : success
	 * - -1 : failed
	 */
	int8_t setBackground(Color color);

	/**
	 * @brief Set background image content for a given side.
	 *
	 * @param bg_image Background image content.
	 * @param side Target eye side.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : null image pointer
	 */
	int8_t setBackground(VContent* bg_image, EyeSide side);

	/**
	 * @brief Start a named eye animation (non-blocking).
	 *
	 * The animation runs on an internal worker thread.
	 * Start/complete/abort are reported via EyeEvent/EyeEventListener.
	 *
	 * @param id User-defined id forwarded to event callbacks.
	 * @param name Animation name (see EyeExpressions).
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : can not found animation, check name
	 */
	int8_t setAnimation(uint16_t id, std::string_view name);

	/**
	 * @brief Set iris position/scale and eyelid limits for a given side.
	 *
	 * @param side Eye side.
	 * @param centerX Iris center X location (-250..0..250).
	 * @param centerY Iris center Y location (-250..0..250).
	 * @param scaleX Scale factor for iris image (default 1).
	 * @param scaleY Scale factor for iris image (default 1).
	 * @param lid_top_end Top eyelid Y end position.
	 * @param lid_bot_start Bottom eyelid Y start position.
	 *
	 * @note Coordinate reference: top-left of LCD is (0,0); center is (120,120).
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : centerX or centerY out of range (-250..0..250)
	 */
	int8_t setIrisPosition(EyeSide side, int16_t centerX, int16_t centerY, float scaleX = 1, float scaleY = 1, uint8_t lid_top_end = 0, uint8_t lid_bot_start = 240);

	/**
	 * @brief Get the current iris center position for a given side.
	 *
	 * @param side Target eye side (LEFT or RIGHT).
	 * @param x Output X coordinate.
	 * @param y Output Y coordinate.
	 *
	 * @return Status code:
	 * - 0  : success
	 * - -1 : wrong side (accepts Left or Right)
	 *
	 * @note The top-left corner of each display is defined as x=0, y=0.
	 */
	int8_t getIrisPosition(EyeSide side, int16_t& x, int16_t& y);

	/**
	 * @brief Get current library version.
	 *
	 * Original note: format 0.XYZ (3 digits after major).
	 *
	 * @return Version as float.
	 */
	float getVersion();

};

/**
 * @brief Named eye expression/animation identifiers.
 *
 * These names are intended to be used with EyeControl::setAnimation().
 */
namespace EyeExpressions
{
	inline constexpr std::string_view ADMIRING = "ADMIRING";
	inline constexpr std::string_view AGGRAVATED = "AGGRAVATED";
	inline constexpr std::string_view ANNOYED = "ANNOYED";
	inline constexpr std::string_view ANXIOUS = "ANXIOUS";
	inline constexpr std::string_view ATTENTION = "ATTENTION";
	inline constexpr std::string_view ATTENTION_LEFT = "ATTENTION LEFT";
	inline constexpr std::string_view ATTENTION_RIGHT = "ATTENTION RIGHT";
	inline constexpr std::string_view AWAKE_L = "AWAKE L";
	inline constexpr std::string_view AWAKE_R = "AWAKE R";
	inline constexpr std::string_view BATTERY_LOW = "BATTERY LOW";
	inline constexpr std::string_view BLINK = "BLINK";
	inline constexpr std::string_view BLINK_BIG = "BLINK BIG";
	inline constexpr std::string_view BLINK_L = "BLINK L";
	inline constexpr std::string_view BLINK_ONLY = "BLINK ONLY";
	inline constexpr std::string_view BLINK_R = "BLINK R";
	inline constexpr std::string_view BLINK_SLOW = "BLINK SLOW";
	inline constexpr std::string_view BLUE = "BLUE";
	inline constexpr std::string_view BUGGED = "BUGGED";
	inline constexpr std::string_view BUMP = "BUMP";
	inline constexpr std::string_view BURNED_UP = "BURNED UP";
	inline constexpr std::string_view CAUTIOUS = "CAUTIOUS";
	inline constexpr std::string_view CAUTIOUS_DOWN = "CAUTIOUS DOWN";
	inline constexpr std::string_view CAUTIOUS_DOWN_LEFT = "CAUTIOUS DOWN LEFT";
	inline constexpr std::string_view CAUTIOUS_DOWN_RIGHT = "CAUTIOUS DOWN RIGHT";
	inline constexpr std::string_view CAUTIOUS_LEFT = "CAUTIOUS LEFT";
	inline constexpr std::string_view CAUTIOUS_RIGHT = "CAUTIOUS RIGHT";
	inline constexpr std::string_view CAUTIOUS_UP = "CAUTIOUS UP";
	inline constexpr std::string_view CHAOTIC = "CHAOTIC";
	inline constexpr std::string_view CHEERFUL = "CHEERFUL";
	inline constexpr std::string_view COLOR_CHANGE = "COLOR CHANGE";
	inline constexpr std::string_view CONCENTRATE = "CONCENTRATE";
	inline constexpr std::string_view CONFUSED = "CONFUSED";
	inline constexpr std::string_view CRAZY_ABOUT = "CRAZY ABOUT";
	inline constexpr std::string_view CRUSHED = "CRUSHED";
	inline constexpr std::string_view CYCLOPS_L = "CYCLOPS L";
	inline constexpr std::string_view CYCLOPS_R = "CYCLOPS R";
	inline constexpr std::string_view DAMAGED = "DAMAGED";
	inline constexpr std::string_view DEJECTED = "DEJECTED";
	inline constexpr std::string_view DELIGHTED = "DELIGHTED";
	inline constexpr std::string_view DEMORALIZED = "DEMORALIZED";
	inline constexpr std::string_view DEPRESSED = "DEPRESSED";
	inline constexpr std::string_view DESTROYED = "DESTROYED";
	inline constexpr std::string_view DISCOVER = "DISCOVER";
	inline constexpr std::string_view DISAPPOINTED = "DISAPPOINTED";
	inline constexpr std::string_view DIZZY_L = "DIZZY L";
	inline constexpr std::string_view DIZZY_R = "DIZZY R";
	inline constexpr std::string_view DOWN = "DOWN";
	inline constexpr std::string_view DROP_CENTER = "DROP CENTER";
	inline constexpr std::string_view DROWSY = "DROWSY";
	inline constexpr std::string_view ELEVATED_I = "ELEVATED I";
	inline constexpr std::string_view ELEVATED_O = "ELEVATED O";
	inline constexpr std::string_view EMPTY = "EMPTY";
	inline constexpr std::string_view EXCITED = "EXCITED";
	inline constexpr std::string_view FED_UP = "FED UP";
	inline constexpr std::string_view FINE = "FINE";
	inline constexpr std::string_view FIREMAN = "FIREMAN";
	inline constexpr std::string_view FLAME = "FLAME";
	inline constexpr std::string_view FOCUS = "FOCUS";
	inline constexpr std::string_view FRIGHTENED = "FRIGHTENED";
	inline constexpr std::string_view FRUSTRATED = "FRUSTRATED";
	inline constexpr std::string_view FUMING_L = "FUMING L";
	inline constexpr std::string_view FUMING_R = "FUMING R";
	inline constexpr std::string_view FURIOUS = "FURIOUS";
	inline constexpr std::string_view GET_BIGGER = "GET BIGGER";
	inline constexpr std::string_view GET_SMALLER = "GET SMALLER";
	inline constexpr std::string_view HAPPY = "HAPPY";
	inline constexpr std::string_view HEARTS = "HEARTS";
	inline constexpr std::string_view HEATED = "HEATED";
	inline constexpr std::string_view HOPELESS = "HOPELESS";
	inline constexpr std::string_view HOSTILE = "HOSTILE";
	inline constexpr std::string_view IMPACT_F = "IMPACT F";
	inline constexpr std::string_view IMPACT_L = "IMPACT L";
	inline constexpr std::string_view IMPACT_R = "IMPACT R";
	inline constexpr std::string_view IMPATIENT = "IMPATIENT";
	inline constexpr std::string_view INJURED = "INJURED";
	inline constexpr std::string_view IRRITATED = "IRRITATED";
	inline constexpr std::string_view JEALOUS_L = "JEALOUS L";
	inline constexpr std::string_view JEALOUS_R = "JEALOUS R";
	inline constexpr std::string_view LEFT = "LEFT";
	inline constexpr std::string_view LIDS_DOWN_5S = "LIDS DOWN 5S";
	inline constexpr std::string_view LOOK_AHEAD = "LOOK AHEAD";
	inline constexpr std::string_view LOOK_AHEAD_SLOW = "LOOK AHEAD SLOW";
	inline constexpr std::string_view LOOK_DOWN = "LOOK DOWN";
	inline constexpr std::string_view LOOK_DOWN_L = "LOOK DOWN L";
	inline constexpr std::string_view LOOK_DOWN_R = "LOOK DOWN R";
	inline constexpr std::string_view LOOK_LEFT = "LOOK LEFT";
	inline constexpr std::string_view LOOK_RIGHT = "LOOK RIGHT";
	inline constexpr std::string_view LOOK_UP = "LOOK UP";
	inline constexpr std::string_view LOOK_UP_L = "LOOK UP L";
	inline constexpr std::string_view LOOK_UP_R = "LOOK UP R";
	inline constexpr std::string_view LOW = "LOW";
	inline constexpr std::string_view LOW_LONG = "LOW LONG";
	inline constexpr std::string_view MELANCHOLY = "MELANCHOLY";
	inline constexpr std::string_view MID_DOWN = "MID DOWN";
	inline constexpr std::string_view MID_UP = "MID UP";
	inline constexpr std::string_view MID_UP_L = "MID UP L";
	inline constexpr std::string_view MID_UP_R = "MID UP R";
	inline constexpr std::string_view MIXED_UP = "MIXED UP";
	inline constexpr std::string_view MOODY_LR = "MOODY LR";
	inline constexpr std::string_view MOODY_RL = "MOODY RL";
	inline constexpr std::string_view NERVOUS = "NERVOUS";
	inline constexpr std::string_view OFFENDED = "OFFENDED";
	inline constexpr std::string_view OUTRAGED = "OUTRAGED";
	inline constexpr std::string_view OVERJOYED = "OVERJOYED";
	inline constexpr std::string_view PANICKY = "PANICKY";
	inline constexpr std::string_view PASSIONATE = "PASSIONATE";
	inline constexpr std::string_view PHOTO = "PHOTO";
	inline constexpr std::string_view POLICE = "POLICE";
	inline constexpr std::string_view PUZZLED = "PUZZLED";
	inline constexpr std::string_view PUMPKIN = "PUMPKIN";
	inline constexpr std::string_view RIGHT = "RIGHT";
	inline constexpr std::string_view ROLL_UD = "ROLL UD";
	inline constexpr std::string_view SCAN = "SCAN";
	inline constexpr std::string_view SHAKE_FRONT = "SHAKE FRONT";
	inline constexpr std::string_view SHAKE_LR = "SHAKE LR";
	inline constexpr std::string_view SHAKE_UD = "SHAKE UD";
	inline constexpr std::string_view SHOCKED = "SHOCKED";
	inline constexpr std::string_view SHY = "SHY";
	inline constexpr std::string_view SLEEP = "SLEEP";
	inline constexpr std::string_view SLEEPY = "SLEEPY";
	inline constexpr std::string_view SNEEZE = "SNEEZE";
	inline constexpr std::string_view SPARKLING = "SPARKLING";
	inline constexpr std::string_view SQUINT_RL = "SQUINT RL";
	inline constexpr std::string_view STORMING = "STORMING";
	inline constexpr std::string_view SUNGLASS = "SUNGLASS";
	inline constexpr std::string_view SUNGLASS_LR = "SUNGLASS LR";
	inline constexpr std::string_view SUNGLASS_UPR = "SUNGLASS UPR";
	inline constexpr std::string_view THINK = "THINK";
	inline constexpr std::string_view THREATENED_L = "THREATENED L";
	inline constexpr std::string_view THREATENED_R = "THREATENED R";
	inline constexpr std::string_view THRILLED = "THRILLED";
	inline constexpr std::string_view THROWN = "THROWN";
	inline constexpr std::string_view TIRED = "TIRED";
	inline constexpr std::string_view TROUBLED = "TROUBLED";
	inline constexpr std::string_view TURNED_ON = "TURNED ON";
	inline constexpr std::string_view UNCOMFORTABLE = "UNCOMFORTABLE";
	inline constexpr std::string_view UNHAPPY = "UNHAPPY";
	inline constexpr std::string_view UPSET = "UPSET";
	inline constexpr std::string_view VR = "VR";
	inline constexpr std::string_view WAKE_WORD = "WAKE WORD";
	inline constexpr std::string_view WORKOUT = "WORKOUT";
	inline constexpr std::string_view ZOOM_IN = "ZOOM IN";
}