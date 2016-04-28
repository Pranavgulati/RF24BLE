//Special thanks to Dmitry Grinberg for completely inspiring this project

#include "RF24BLE.h"
#define BYTELEN 8

RF24BLE::RF24BLE(RF24& radio) : _radio(radio){

	chRf[0] = 2;
	chRf[1] = 26;
	chRf[2] = 80;
	chLe[0] = 37;
	chLe[1] = 38;
	chLe[2] = 39;
}
void RF24BLE::BLEcrc(const uint8_t* data, uint8_t len, uint8_t* output){
	// implementing CRC with LFSR
	uint8_t i, temp, d;

	while (len--){
		d = *data++;
		for (i = 0; i < 8; i++, d >>= 1){
			temp = output[0] >> 7;
			output[0] <<= 1;
			if (output[1] & 0x80) output[0] |= 1;
			output[1] <<= 1;
			if (output[2] & 0x80) output[1] |= 1;
			output[2] <<= 1;

			if (temp != (d & 1)){
				output[2] ^= 0x5B;
				output[1] ^= 0x06;
			}
		}
	}
}
void RF24BLE::bleWhiten(uint8_t* data, uint8_t len, uint8_t whitenCoeff){
	// Implementing whitening with LFSR
	uint8_t  m;
	while (len--){
		for (m = 1; m; m <<= 1){
			if (whitenCoeff & 0x80){
				whitenCoeff ^= 0x11;
				(*data) ^= m;
			}
			whitenCoeff <<= 1;
		}
		data++;
	}
}
void RF24BLE::blePacketEncode(uint8_t* packet, uint8_t len, uint8_t chan){
	// Assemble the packet to be transmitted
	// Packet length includes pre-populated crc 
	uint8_t i, dataLen = len - 3;
	BLEcrc(packet, dataLen, packet + dataLen);
	for (i = 0; i < 3; i++, dataLen++)
		packet[dataLen] = reverseBits(packet[dataLen]);
	bleWhiten(packet, len, bleWhitenStart(chan));
	for (i = 0; i < len; i++)
		packet[i] = reverseBits(packet[i]); // the byte order of the packet should be reversed as well

}
void RF24BLE::begin(){

	_radio.disableCRC();
	_radio.powerUp();
	_radio.setAutoAck(false);
	_radio.disableRX();
	_radio.setAddressLength(4);
	_radio.setRetries(0, 0);
	_radio.setDataRate(RF24_1MBPS);//already done by begin
	_radio.setPALevel(RF24_PA_MAX);//already done by begin
	// Set access addresses (TX address in nRF24L01) to BLE advertising 0x8E89BED6
	// Both bit and byte orders are reversed for BLE packet format
	unsigned long address;
	address = reverseBits(0xD6);
	address <<= BYTELEN;
	address |= reverseBits(0xBE);
	address <<= BYTELEN;
	address |= reverseBits(0x89);
	address <<= BYTELEN;
	address |= reverseBits(0x8E);
	_radio.openWritingPipe(address, 4);

}
void RF24BLE::setPhone(uint8_t phone_type){
	//byte no.0 PDu
	_packet[0] = phone_type;
}
void RF24BLE::setMAC(uint8_t m0, uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4, uint8_t m5){
	//length of payload is entered in byte no.1
	_L = 2;
	_packet[_L++] = m0;
	_packet[_L++] = m1;
	_packet[_L++] = m2;
	_packet[_L++] = m3;
	_packet[_L++] = m4;
	_packet[_L++] = m5;
	//L should be 8 by now
	_packet[_L++] = 2;		//flags (LE-only, limited discovery mode)
	_packet[_L++] = 0x01;
	_packet[_L++] = 0x05;
}

void RF24BLE::setName(char* name){
	//8,9,10 bytes are for flags 
	//name field starts from 11th byte
	//Serial.print(name);
	//Serial.println(strlen(name));
	_packet[_L++] = strlen(name) + 1;
	_packet[_L++] = 0x08;//name type 
	for (uint8_t i = 0; i < strlen(name); i++){
		_packet[_L++] = name[i];
	}

}

void RF24BLE::setData(const void* data,uint8_t dataLen){

	const uint8_t* current = reinterpret_cast<const uint8_t*>(data);
	//8,9,10 bytes are for flags 
	//name field starts from 11th byte
	Serial.print("data "); Serial.println(dataLen);
	_packet[_L++] = dataLen + 1;
	_packet[_L++] = 0xFF;//data type 
	for (uint8_t i = 0; i < dataLen; i++){
		_packet[_L++] = *(current++);
	}
	//after setting data only CRC is left append that to the data
	_packet[_L++] = 0x55;	//CRC starting val 0x555555
	_packet[_L++] = 0x55;
	_packet[_L++] = 0x55;
}

void RF24BLE::sendADV(uint8_t channel){
	//Serial.print("length "); Serial.println(L);
	_radio.setChannel(chRf[channel]);
	_packet[1] = _L-5;//subtract checksum bytes and the 2 bytes including the length byte itself
	blePacketEncode(_packet, _L, chLe[channel]);
	_radio.startWrite(_packet, _L);

}