"""
example.py

It demonstrates:
- Initializing the TTS (text to speech) controller
- Produce sound
- Initializing the sound controller
- Playin sound
- Cleaning up (dispose)

"""

import time
import doly_helper as helper
import doly_sound as snd
import doly_tts as tts

def main():

    # Version
    try:
        print(f"[info] TtsControl Version: {tts.get_version():.3f}")
    except AttributeError:
        pass

    # *** IMPORTANT *** 
    # Stop doly service if running,
    # otherwise instance of libraries cause conflict	
    if helper.stop_doly_service() < 0: 
        print("[error] Doly service stop failed")
        return -1
    
    # Initialize tts controler
    print("[info] Initialize and load voice model...")
    rc = tts.init(tts.VoiceModel.Model1, "output.wav")
    if rc < 0:
        print(f"[error] TtsControl.init failed rc={rc}")
        return -2
    
    # Generate speech sound file from text
    print("[info] Produce Text to Speech")
    tts.produce("Hello, this is a text to speech test example.")
    
    # Initialize sound controler to test sound file
    rc = snd.init()
    if rc < 0:
        print(f"[error] SoundControl.init failed rc={rc}")
        return -3
    
    # Play sound
    snd.play("output.wav", 1)

    # wait for sound to complete
    time.sleep(5)

    # Cleanup
    tts.dispose()
    snd.dispose()    
    time.sleep(0.2) 

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
