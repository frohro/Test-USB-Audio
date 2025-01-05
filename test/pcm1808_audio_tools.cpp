/**
 * @brief We define a custom board with the i2c and i2s pins and output_device a sine
 * with the help of the AudioTools I2SCodecStream
 * @author phil schatzmann
 */

#include "AudioTools.h" // install https://github.com/pschatzmann/arduino-audio-tools
#include "AudioTools/AudioLibs/I2SCodecStream.h"


// I2S pin configuration for PCM1808 ADC
#define MCLKPIN              3    // SCKI (3) - System/Master Clock Input
#define BCLKPIN              0    // BCK (0) - Bit Clock
#define WSPIN                1    // LRCLK (1) - Word Select/Left-Right Clock
#define DOPIN                2    // DATA (2) - Serial Data Output from ADC
// DIPIN removed as PCM1808 is ADC only

AudioInfo                     audio_info(48000, 2, 16);                // sampling rate, # channels, bit depth
DriverPins                    my_pins;
NoDriverClass                 NoDriver; 
GeneratedSoundStream<int16_t> sound_stream;                                // board pins
AudioBoard                    audio_board(NoDriver, my_pins); // audio board
I2SCodecStream                i2s_in_stream(audio_board);             // i2s coded
StreamCopy                    copier(sound_stream, i2s_in_stream);    // stream copy sound generator to i2s codec

void setup() {
  // Setup logging
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);
  LOGLEVEL_AUDIODRIVER = 0;
  delay(2000);

  Serial.println("Setup starting...");
  
  Serial.println("I2S pin ...");
  my_pins.addI2S(PinFunction::CODEC, MCLKPIN, BCLKPIN, WSPIN, DOPIN, DIPIN);

  Serial.println("Pins begin ..."); 
  my_pins.begin();

  Serial.println("Board begin ..."); 
  audio_board.begin();

  Serial.println("I2S begin ..."); 
  auto i2s_config = i2s_out_stream.defaultConfig();
  i2s_config.copyFrom(audio_info);  
  i2s_out_stream.begin(i2s_config); // this should apply I2C and I2S configuration

  // Setup sine wave
  Serial.println("Sine wave begin...");
  sine_wave.begin(audio_info, N_B4); // 493.88 Hz

  Serial.println("Setup completed ...");
}

// Arduino loop - copy sound to out
void loop() { copier.copy(); }