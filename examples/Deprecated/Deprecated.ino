#include <RF24.h>
#include <RF24BLE.h>

#define PIN_CE  9    // chip enable
#define PIN_CS  10   // chip select (for SPI)

RF24 radio(PIN_CE, PIN_CS);
RF24BLE BLE(radio);

// The MAC address of the beacon
#define MY_MAC_0  'A'
#define MY_MAC_1  'B'
#define MY_MAC_2  'C'
#define MY_MAC_3  'D'
#define MY_MAC_4  'E'
#define MY_MAC_5  'F'

void setup() {
  Serial.begin(115200);
  // SPI in Arduino Uno/Nano: MOSI pin 11, MISO pin 12, SCK pin 13
  BLE.begin();
}

void loop() {
  // This method of advertising using the channel no. is deprecated
  // See other examples
  uint8_t dataBytes[] = { "hello" };
  for (uint8_t channel = 0; channel < 3; channel++){  // Channel hopping do not alter
    BLE.setPhone(PHONE_ANDROID);
    BLE.setMAC(MY_MAC_0, MY_MAC_1, MY_MAC_2, MY_MAC_3, MY_MAC_4, MY_MAC_5);
    BLE.setName("nrf2");
    BLE.setData(dataBytes, sizeof(dataBytes));
    BLE.sendADV(channel);
    Serial.print("sent bytes: ");Serial.println(  BLE.getPacketLengthCurr());
    delay(1);    // Required between each channel change
  }
  delay(1);
}


