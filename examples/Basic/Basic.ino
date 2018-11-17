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
  // SPI in Arduino Uno/Nano: MOSI pin 11, MISO pin 12, SCK pin 13
  Serial.begin(115200);
  // setPhone,setMAC,setName must always be called only once
  BLE.begin();
  BLE.setPhone(PHONE_ANDROID);
  BLE.setMAC(MY_MAC_0, MY_MAC_1, MY_MAC_2, MY_MAC_3, MY_MAC_4, MY_MAC_5);
  BLE.setName("nrf2");

}

void loop() {  
  uint8_t dataBytes[] = { "hello" };
  BLE.setData(dataBytes, sizeof(dataBytes));
  //RF24BLE.advertise() blocks for atleast 3 milliseconds
  BLE.advertise();
  delay(2);
  Serial.print("sent bytes: ");
  Serial.println(BLE.getPacketLengthCurr());
}


