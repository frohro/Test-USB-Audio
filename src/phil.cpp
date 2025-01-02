#include "Adafruit_TinyUSB.h";
#include "AudioTools.h"

AudioInfo info(48000, 2, 16);
SineWaveGenerator<int16_t> sineWave(32000);                // subclass of SoundGenerator with max amplitude of 32000
GeneratedSoundStream<int16_t> sound(sineWave);             // Stream generated from sine wave
Adafruit_USBD_Audio usb;

size_t readCB(uint8_t* data, size_t len, Adafruit_USBD_Audio& ref) {
  return sound.readBytes(data, len);
}

void setup() {
  Serial.begin(115200);

  // Setup sine wave
  sineWave.begin(info, 20000.0);

  // Start USB device as Audio Source
  usb.setReadCallback(readCB);
  usb.begin(info.sample_rate, info.channels, info.bits_per_sample);

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}

void loop() {
  // use LED do display status
  usb.updateLED();
}