#include "SPI.h"  // SPI in Arduino Uno/Nano: MOSI pin 11, MISO pin 12, SCK pin 13
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <RF24BLE.h>

#define PIN_CE  9 // chip enable
#define PIN_CS  10   // chip select (for SPI)
RF24 radio(PIN_CE, PIN_CS);
RF24BLE BLE(radio);
// The MAC address of the beacon
//anything goes but should be constant over time
#define MY_MAC_0  'A'
#define MY_MAC_1  'A'
#define MY_MAC_2  'A'
#define MY_MAC_3  'A'
#define MY_MAC_4  'A'
#define MY_MAC_5  'B'
#define ANDROID    0x42
#define IPHONE    0x40


void setup() {
  Serial.begin(115200);
  SPI.begin();
  radio.begin();
  BLE.begin();
}

void loop() {
  
  for (uint8_t channel = 0; channel < 3; channel++){  // Channel hopping do not alter
    BLE.setPhone(ANDROID);
    BLE.setMAC(MY_MAC_0, MY_MAC_1, MY_MAC_2, MY_MAC_3, MY_MAC_4, MY_MAC_5);
    BLE.setName("nrf2");
    uint8_t dataBytes[] = { "hello" };
    BLE.setData(dataBytes, sizeof(dataBytes));
    BLE.sendADV(channel);
    Serial.print("sent bytes: ");Serial.println(BLE.getPacketLengthCurr());
    delay(1);    // Broadcasting interval
  }
  delay(1);
}

