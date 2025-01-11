
#include "Adafruit_TinyUSB.h"
#include "AudioTools.h"
#include "AudioTools/Concurrency/RP2040.h"

AudioInfo info(48000, 2, 16);
Adafruit_USBD_Audio usb;
BufferRP2040 buffer(256, 20); // Thread safe circular buffer
QueueStream queue(buffer);
I2SStream i2s;
StreamCopy copier(queue, i2s);

size_t readCB(uint8_t *data, size_t len, Adafruit_USBD_Audio &ref)
{
  size_t bytes_read = 0;
  while (bytes_read < len && queue.available())
  {
    data[bytes_read] = queue.read();
    bytes_read++;
  }
  usb.setFeedbackPercent(buffer.size() * 100 / buffer.available());
  return bytes_read;
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(100);
  } // Block until Serial ready
  Serial.println("\n\nCore0: Starting USB Audio");
  rp2040.fifo.push(0xAA); // Signal Core1
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Warning);
  // while(!Serial);  // wait for serial
  delay(1000);
  Serial.println("starting...");
  // start queue
  queue.begin(80);
  Serial.println("Queue started");

  // Start USB device as Audio Source
  usb.setFeedbackMethod(AUDIO_FEEDBACK_METHOD_DISABLED);
  usb.setReadCallback(readCB);
  if (!usb.begin(info.sample_rate, info.channels, info.bits_per_sample))
  {
    Serial.println("USB error");
  }
  Serial.println("USB started");

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted())
  {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }
}

void loop()
{
  static uint32_t last_print = 0;
  if (millis() - last_print > 1000)
  {
    Serial.printf("Buffer: %d/%d\n", buffer.size(), buffer.available());
    last_print = millis();
  }
  usb.updateLED();
}

void setup1()
{
  while (rp2040.fifo.pop() != 0xAA)
  {
    delay(10);
  } // Wait for Core0
  Serial.println("Core1: Starting I2S");
  auto cfg = i2s.defaultConfig(RX_MODE);
  cfg.copyFrom(info);
  cfg.pin_data = 2;
  cfg.pin_bck = 0;
  cfg.pin_ws = 1;
  cfg.pin_mck = 3;
  cfg.buffer_size = 256;
  cfg.buffer_count = 3;
  cfg.i2s_format = I2S_PHILIPS_FORMAT;
  cfg.bits_per_sample = 32; // Explicitly set to 32
  cfg.mck_multiplier = 256; // Add MCLK multiplier

  if (!i2s.begin(cfg))
  {
    Serial.print("i2s error");
  }
  Serial.println("I2S started");
}

void loop1()
{
  copier.copy();
}
