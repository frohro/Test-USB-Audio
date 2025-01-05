#include <cmath>
#include "Adafruit_TinyUSB.h"
#include "AudioTools.h"
#include <Arduino.h>
#include <I2S.h>

I2S i2s;
const int RATE = 48000;              // Your network needs to handle this, but 96000 should also work, but misses some packets.
const int BITS_PER_SAMPLE_SENT = 16; // 24 or 32, 32 is less packet loss for some strange reason.
const int MCLK_MULT = 256;  

Adafruit_USBD_Audio usb;

size_t readCB(uint8_t* data, size_t len, Adafruit_USBD_Audio& ref) {
  int16_t* data16 = (int16_t*)data;
  // Serial.println(len);
  size_t samples = len / sizeof(int16_t);
  // Serial.println(samples);
  size_t result = 0;
  int32_t r[samples], l[samples];

  for (size_t j = 0; j < samples-1; j += 2) {  // -1 is a work around for a bug in the library
    int k = j/2;
    i2s.read32(l + k, r + k);
    data16[j] = (l[k] << 8) & 0xFFFF;       // Take the lower 16 bits of l
    data16[j+1] = (r[k] << 8) & 0xFFFF;     // Take the lower 16 bits of r
    result += sizeof(int16_t) * 2;
  }
  Serial.printf("l: %ld, r: %ld\n", l, r);
  return result;
}

void setup() {
  Serial.begin(115200);

    i2s.setDATA(2); // These are the pins for the data on the SDR-TRX
    i2s.setBCLK(0);
    i2s.setMCLK(3);
    // Note: LRCK pin is always BCK pin plus 1 (1 in this case).
    i2s.setSysClk(RATE);
    i2s.setBitsPerSample(24);
    i2s.setFrequency(RATE);
    i2s.setMCLKmult(MCLK_MULT);
    i2s.setBuffers(32, 0, 0);
    i2s.begin();

  // Start USB device as Microphone
  usb.setReadCallback(readCB);
  usb.begin(RATE, 2, 16); 

  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}

void loop() {
  // optionally use LED to display status
  usb.updateLED();
}