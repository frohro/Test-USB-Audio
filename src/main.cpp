#include "Adafruit_TinyUSB.h"
#include "AudioTools.h"

const float sampleRate = 48000;

int16_t triangleWave = 0;

Adafruit_USBD_Audio usb;

size_t readCB(uint8_t* data, size_t len, Adafruit_USBD_Audio& ref) {
  int16_t* data16 = (int16_t*)data;
  size_t samples = len / sizeof(int16_t);
  size_t result = 0;
  static bool up = true;
  Serial.printf("samples: %d\n", samples);
  
  for (size_t j = 0; j < samples; j += 2) {
    if (up) {
      triangleWave += 1000;
      if (triangleWave >= 30000) {
        up = false;
      }
    } else {
      triangleWave -= 1000;
      if (triangleWave <= 0) {
        up = true;
      }
    }
    Serial.printf("triangleWave: %d\n", triangleWave);
    data16[j] = triangleWave; // Left channel  
    data16[j + 1] = triangleWave; // Right channel
    result += sizeof(int16_t) * 2;
  }
  Serial.printf("result: %d\n", result);
  return result;
}

void setup() {
  Serial.begin(115200);

  // Start USB device as Microphone
  usb.setReadCallback(readCB);
  usb.begin(sampleRate, 2, 16); 

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