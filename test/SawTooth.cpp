#include "Adafruit_TinyUSB.h"
#include "AudioTools.h"  // https://github.com/pschatzmann/arduino-audio-tools

Adafruit_USBD_Audio usb;
AudioInfo info(48000, 2, 16);
SawToothGenerator<int16_t> sawtooth;               
GeneratedSoundStream<int16_t> sawthooth_stream(sawtooth);

void setup() {
  Serial.begin(115200);
  //while(!Serial);  // wait for serial

  // generate 493 hz (note B4)
  sawtooth.begin(info, 493.0f);

  // Start USB device as Audio Source
  usb.setInput(sawthooth_stream);
  usb.begin(info.sample_rate, info.channels, info.bits_per_sample);
}

void loop() {
  // optional: use LED do display status
  usb.updateLED();
}