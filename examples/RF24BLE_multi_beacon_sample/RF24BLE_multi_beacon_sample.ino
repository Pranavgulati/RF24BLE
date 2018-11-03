#include "SPI.h"  // SPI in Arduino Uno/Nano: MOSI pin 11, MISO pin 12, SCK pin 13
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <RF24BLE.h>

#define PIN_CE  9 // chip enable
#define PIN_CS  10   // chip select (for SPI)
RF24 radio(PIN_CE, PIN_CS);
//special initialization such that each instance gets its radio object
RF24BLE BLE[2] = {{radio},{radio}};

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
  BLE[0].begin();
  BLE[0].setPhone(ANDROID);
  BLE[0].setMAC(MY_MAC_0, MY_MAC_1, MY_MAC_2, MY_MAC_3, MY_MAC_4, MY_MAC_5);
  BLE[0].setName("nrf2");
  BLE[1].begin();
  BLE[1].setPhone(PHONE_IPHONE);
  BLE[1].setMAC(MY_MAC_0, MY_MAC_1, MY_MAC_2, MY_MAC_3, MY_MAC_5, MY_MAC_4);
  BLE[1].setName("nrf3");
}

void loop() {
  
      uint8_t dataBytes[] = { "hello" };
      BLE[0].setData(dataBytes, sizeof(dataBytes));
      Serial.print("sent bytes: ");Serial.println(  BLE[0].getPacketLengthCurr());
      BLE[0].advertise();


      uint8_t dataBytes2[] = { "helol" };
      BLE[1].setData(dataBytes, sizeof(dataBytes));
      Serial.print("sent bytes: ");Serial.println(  BLE[0].getPacketLengthCurr());
      BLE[1].advertise();

}


