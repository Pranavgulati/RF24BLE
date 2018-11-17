#include <RF24.h>
#include <RF24BLE.h>

#define PIN_CE  9    // chip enable
#define PIN_CS  10   // chip select (for SPI)
RF24 radio(PIN_CE, PIN_CS);
//special initialization such that each instance gets its radio object
RF24BLE BLE[2] = {{radio},{radio}};

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
  radio.begin();
  // setPhone,setMAC,setName must always be called only once
  BLE[0].begin();
  BLE[0].setPhone(PHONE_ANDROID);
  BLE[0].setMAC(MY_MAC_0, MY_MAC_1, MY_MAC_2, MY_MAC_3, MY_MAC_4, MY_MAC_5);
  BLE[0].setName("nrf2");
  BLE[1].begin();
  BLE[1].setPhone(PHONE_ANDROID);
  BLE[1].setMAC(MY_MAC_0, MY_MAC_1, MY_MAC_2, MY_MAC_3, MY_MAC_5, MY_MAC_4);
  BLE[1].setName("nrf3");
}

void loop() {  
  
  uint8_t dataBytes1[] = { "hello" };
  uint8_t dataBytes2[] = { "world" };
  BLE[0].setData(dataBytes1, sizeof(dataBytes1));
  //RF24BLE.advertise() blocks for atleast 4 milliseconds
  BLE[0].advertise();
  delay(2);
  BLE[1].setData(dataBytes2, sizeof(dataBytes2));
  BLE[1].advertise();
  delay(2);
  Serial.print("sent bytes: ");
  Serial.println(BLE[0].getPacketLengthCurr());
}


