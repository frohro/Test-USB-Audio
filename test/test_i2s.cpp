#include <Arduino.h>
#include <I2S.h>

I2S i2s(INPUT);

int32_t r[500], l[500];

void setup()
{

    Serial.begin();

    i2s.setDATA(2);
    i2s.setBCLK(0);
    i2s.setMCLK(3);
    // Note: LRCK pin is BCK pin plus 1 (1 in this case).
    i2s.setBitsPerSample(24);
    i2s.setFrequency(8000);
    i2s.setMCLKmult(384);
    i2s.setSysClk(8000);
    i2s.setBuffers(64, 16, 0);
    i2s.begin();
}

void loop()
{
    int i;
    for (i = 0; i < 500; i++)
    {
        i2s.read32(l + i, r + i);
    }
    for (i = 0; i < 500; i++)
    {
        Serial.printf("> Right:%d, Left:%d\r\n", l[i] << 8, r[i] << 8);
    }
}