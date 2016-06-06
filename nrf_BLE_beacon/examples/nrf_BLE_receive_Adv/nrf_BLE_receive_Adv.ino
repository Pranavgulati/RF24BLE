/*
code for receiving valid BLE advertisement packets 
*/
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <RF24BLE.h>
#include "printf.h"

#define DEBUG 0
#define PIN_CE  9 // chip enable
#define PIN_CS  10   // chip select (for SPI)
// The MAC address of the beacon
//anything goes but should be constant over time
#define MY_MAC_0  'A'
#define MY_MAC_1  'A'
#define MY_MAC_2  'A'
#define MY_MAC_3  'A'
#define MY_MAC_4  'A'
#define MY_MAC_5  'B'
//EXPECTED receive packet length must be same as that of the length of the sent packet
#define RECV_PAYLOAD_SIZE 29

//do not change default channels for BLE
uint8_t chRF[3]={0x02,0x1A,0x50};
uint8_t chBLE[3]={37,38,39};

RF24 radio(PIN_CE, PIN_CS);
RF24BLE BLE(radio);


void setup() {
  Serial.begin(115200);
  SPI.begin();
  radio.begin();
  printf_begin();
  //initialize the radio for receiving BLE advertisements
  BLE.recvBegin(RECV_PAYLOAD_SIZE,chRF[0]);
}

void loop() {
    char input[32]={0};
    //receive shall be done when called
    //single channel receiving only 
    //since every channel will have the same information hence choose the optimum channel as per your envt.
    byte status=BLE.recvPacket((uint8_t*)input,RECV_PAYLOAD_SIZE,chBLE[0]);
    if(status==RF24BLE_VALID){Serial.println("VALID");}//RF24BLE_VALID=1
    else if(status==RF24BLE_CORRUPT){ Serial.println("CORRUPT");}//RF24BLE_CORRUPT =0
    else if(status==RF24BLE_TIMEOUT){ Serial.println("TIMEOUT");}//RF24BLE_TIMEOUT =-1
#if DEBUG 
  for (byte i = 0; i < RECV_PAYLOAD_SIZE; i++){
    Serial.print((char)input[i]);
  }Serial.println();
  radio.printDetails();
#endif
//delay(1);//breathing time
}
 
  

