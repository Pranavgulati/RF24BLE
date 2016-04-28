

#include "SPI.h"  // SPI in Arduino Uno/Nano: MOSI pin 11, MISO pin 12, SCK pin 13
#include <nRF24L01.h>
#include <RF24BLE.h>
#include <RF24.h>
#include "printf.h"
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


void setup() {
	pinMode(PIN_CS, OUTPUT);
	pinMode(PIN_CE, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(13, OUTPUT);
	digitalWrite(PIN_CS, HIGH);
	digitalWrite(PIN_CE, LOW);
	Serial.begin(115200);
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	radio.begin();
	printf_begin();
	BLE.begin();

	
}

void loop() {
	for (uint8_t channel = 0; channel < 3; channel++){	// Channel hopping do not alter
		uint8_t dataLen = 5;
		Serial.println("Start LE advertizing");
		BLE.setPhone(ANDROID);
		BLE.setMAC(MY_MAC_0, MY_MAC_1, MY_MAC_2, MY_MAC_3, MY_MAC_4, MY_MAC_5);
		BLE.setName("nrf2");
		uint8_t dataBytes[MAX_DATA_SIZE] = { "hello" };
		BLE.setData(dataBytes, dataLen);
		BLE.sendADV(channel);
		//radio.printDetails();
		delay(500);    // Broadcasting interval
	}
	
}
