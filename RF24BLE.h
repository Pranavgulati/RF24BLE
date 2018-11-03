#ifndef __RF24BLE_H__
#define __RF24BLE_H__
#include "RF24.h"
#define DEBUG 0
#define RECV_TIMEOUT 200

#define RF24BLE_VALID 1
#define RF24BLE_CORRUPT 0
#define RF24BLE_TIMEOUT 255

//using an enum might be good if we have more types
#define PHONE_ANDROID   0x42
#define PHONE_IPHONE    0x40

class RF24BLE{


	static const byte chRf[];
	static const byte chLe[];
	RF24& _radio;
private:
	uint8_t _packet[32]; //maximum size of payload handled by nrf24L01
	uint8_t _length = 0; //length of packet filled


	static uint8_t  reverseBits(uint8_t input){
		// reverse the bit order in a single byte
		uint8_t temp = 0;
		if (input & 0x80) temp |= 0x01;
		if (input & 0x40) temp |= 0x02;
		if (input & 0x20) temp |= 0x04;
		if (input & 0x10) temp |= 0x08;
		if (input & 0x08) temp |= 0x10;
		if (input & 0x04) temp |= 0x20;
		if (input & 0x02) temp |= 0x40;
		if (input & 0x01) temp |= 0x80;
		return temp;
	}
	static inline uint8_t  bleWhitenStart(uint8_t chan){
		//use left shifted one
		return reverseBits(chan) | 2;
	}

protected:
	void  blePacketEncode(uint8_t* packet, uint8_t len, uint8_t chan);
	void  bleWhiten(uint8_t* data, uint8_t len, uint8_t whitenCoeff);
	void  BLEcrc(const uint8_t* data, uint8_t len, uint8_t* output);
	uint8_t checkCRC(uint8_t *input,uint8_t length);
public:
	RF24BLE(RF24& radio);
	void begin();
	void transmitBegin();
	void recvBegin(uint8_t payloadSize, uint8_t channel, uint64_t pipeAddress = 0xe76b7d9171);
	void setMAC(uint8_t m0, uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4, uint8_t m5);
	void setPhone(uint8_t phone_type);
	void setName(const char* name);
	void setData(const void* data,uint8_t dataLen);
	/*
		@brief
		This function is a blocking function and it blocks for at least 4 ms
	*/
	void advertise();
	void sendADV(uint8_t channel);
	void printPacket();
	uint8_t getPacketLengthCurr();//returns the current length of packet
	uint8_t recvPacket(uint8_t *input, uint8_t length, uint8_t channel);
};

#endif