/*to DEBUG change the #define DEBUG 0 to 1 from 0 in RF24BLE
*
* the android app nrF master control from play store can be used to view the transmitted packet
*/
#include "SPI.h"  // SPI in Arduino Uno/Nano: MOSI pin 11, MISO pin 12, SCK pin 13
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24BLE.h>
#include "printf.h"
//MAX_DATA_SIZE depends on whether the setName function is called or not
//to maximize data throughput DO NOT call setName instead directly send relevant data using setData

#define PIN_CE	9 // chip enable
#define PIN_CS	10   // chip select (for SPI)
RF24 radio(PIN_CE, PIN_CS);
RF24BLE BLE(radio);
// The MAC address the beacon
//anything goes
#define MY_MAC_0	'A'
#define MY_MAC_1	'A'
#define MY_MAC_2	'A'
#define MY_MAC_3	'A'
#define MY_MAC_4	'A'
#define MY_MAC_5	'B'
#define ANDROID		 0x42
#define IPHONE		0x40
unsigned long time;

void setup() {
	time = millis();
	Serial.begin(115200);
	SPI.begin();
	radio.begin();
	printf_begin();
	BLE.begin();
	//can be used to know if the radio is connected properly or not
	//radio.printDetails();

}

void loop() {
	time = millis();
	for (uint8_t channel = 0; channel < 3; channel++){	// Channel hopping do not alter
		Serial.println("Start LE advertizing");
		BLE.setPhone(ANDROID);
		BLE.setMAC(MY_MAC_0, MY_MAC_1, MY_MAC_2, MY_MAC_3, MY_MAC_4, MY_MAC_5);
		//may or may not set name 
		//BLE.setName("nrfBle");
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
		BLE.sendADV(channel);
	}
	delay(500);    // Broadcasting interval
}
