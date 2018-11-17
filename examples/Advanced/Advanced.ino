/* 
 *  To DEBUG change the #define DEBUG 0 to 1 from 0 in RF24BLE.h
*/
#include <RF24.h>
#include <RF24BLE.h>
#include "printf.h"
// maximu size of data that can be advertised depends on whether the setName function is called or not
// to maximize data throughput DO NOT call setName instead directly send relevant data using setData

#define PIN_CE	9 // chip enable
#define PIN_CS	10   // chip select (for SPI)

RF24 radio(PIN_CE, PIN_CS);
RF24BLE BLE(radio);

// The MAC address of the beacon
#define MY_MAC_0  'A'
#define MY_MAC_1  'B'
#define MY_MAC_2  'C'
#define MY_MAC_3  'D'
#define MY_MAC_4  'E'
#define MY_MAC_5  'F'

unsigned long time;

void setup() {
	time = millis();
	Serial.begin(115200);
  // SPI in Arduino Uno/Nano: MOSI pin 11, MISO pin 12, SCK pin 13
	printf_begin();
  BLE.begin();
  BLE.setPhone(PHONE_ANDROID);
  BLE.setMAC(MY_MAC_0, MY_MAC_1, MY_MAC_2, MY_MAC_3, MY_MAC_4, MY_MAC_5);
  //may or may not set name 
  //BLE.setName("nrf2");
	//can be used to know if the radio is connected properly or not
	//radio.printDetails();
	
}

void loop() {
	time = millis();
	Serial.println("Advertising");
  /*
  *can use any data type to send data 
  *it is typecasted internally
  *this can also be used to send float or long etc. data type as under
  */
  //float dataBytes = 1.1;
  unsigned long dataBytes = time;
  /*
  *be advised sending a string like this also sends the null termination character
  *use character arrays for more control
  */
  //uint8_t dataBytes[] = { "helloMO" };
  BLE.setData(&dataBytes, sizeof(dataBytes));
  BLE.advertise();
  delay(2);
}
