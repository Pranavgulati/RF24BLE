//Special thanks to Dmitry Grinberg for completely inspiring this project

#include "RF24BLE.h"
#define BYTELEN 8


const byte RF24BLE::chRf[] = {2,26,80} ;
const byte RF24BLE::chLe[] = {37,38,39} ;

RF24BLE::RF24BLE(RF24& radio) : _radio(radio){}

void RF24BLE::BLEcrc(const uint8_t* data, uint8_t len, uint8_t* output){
	//packet ,data length ,CRC output
	// calculating the CRC based on a LFSR
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

uint8_t RF24BLE::checkCRC(uint8_t *input,uint8_t length){
	uint8_t CRC[3] ={0x55,0x55,0x55 }; //initial value for bluetooth crc
	uint8_t dataLen = length - 3;
	BLEcrc(input, dataLen, CRC);
	if (CRC[0] == *(input + dataLen++) && CRC[1] == *(input + dataLen++) && CRC[2] == *(input + dataLen)){
		//PACKET IS VALID
		//Serial.println("VALID");
		return 1;
	}
	else {
		//PACKET is invalid
		//Serial.println("CORRUPT");
		return 0; }
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
	transmitBegin();
}
void RF24BLE::transmitBegin(){
	_radio.disableCRC();
	_radio.powerUp();
	_radio.setAutoAck(false);
	_radio.stopListening();
	_radio.setAddressWidth(4);
	_radio.setRetries(0, 0);
	_radio.setDataRate(RF24_1MBPS);
	_radio.setPALevel(RF24_PA_MAX);
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
	_radio.openWritingPipe(address);
}
void RF24BLE::recvBegin(uint8_t payloadSize, uint8_t channel, uint64_t pipeAddress){
	begin();
	_radio.setChannel(RF24BLE::chRf[channel]);
	_radio.setPayloadSize(payloadSize);
	_radio.openReadingPipe(1, 0xe76b7d9171);
	_radio.startListening();

}

void RF24BLE::setPhone(uint8_t phone_type){
	//byte no.0 PDU
	_packet[0] = phone_type; 
}
void RF24BLE::setMAC(uint8_t m0, uint8_t m1, uint8_t m2, uint8_t m3, uint8_t m4, uint8_t m5){
	//length of payload is entered in byte no.1
	_length = 2;
	_packet[_length++] = m0;
	_packet[_length++] = m1;
	_packet[_length++] = m2;
	_packet[_length++] = m3;
	_packet[_length++] = m4;
	_packet[_length++] = m5;
	//length should be 8 by now
	//flags (LE-only, limited discovery mode)
	_packet[_length++] = 2;    //flag length
	_packet[_length++] = 0x01; //data type
	_packet[_length++] = 0x05; //actual flag
}

void RF24BLE::setName(const char* name){
	
	//8,9,10 bytes are for flags 
	//name field starts from 11th byte
#if DEBUG == 1
	Serial.print(name);
	Serial.print(" ");
	Serial.println(strlen(name));
#endif
	if (strlen(name) != 0){
		//length of name including the terminating null character
		_packet[_length++] = strlen(name) + 1;
		_packet[_length++] = 0x08;//name type short name
		for (uint8_t i = 0; i < strlen(name); i++){
			_packet[_length++] = name[i];
		}
	}
	//else 
		//no name to be sent 
		//directly send manufacturere specific data 0xFF
		//do nothing here
	
}

void RF24BLE::setData(const void* data,uint8_t dataLen){

	const uint8_t* current = reinterpret_cast<const uint8_t*>(data);
	//8,9,10 bytes are for flags 
	//name field starts from 11th byte
#if DEBUG == 1 
	Serial.print("data "); Serial.println(dataLen);
#endif
	_packet[_length++] = dataLen +1;
	_packet[_length++] = 0xFF;//data type
	for (uint8_t i = 0; i < dataLen; i++){
		//Serial.print(*current);
		_packet[_length++] = *(current);
		current++;
		
	}
	//CRC is appended to the data
	//CRC starting val 0x555555 acc. to spec
	_packet[_length++] = 0x55;
	_packet[_length++] = 0x55;
	_packet[_length++] = 0x55;
}

void RF24BLE::advertise(){
	transmitBegin();
	  for (uint8_t channel = 0; channel < 3; channel++){  
	  		sendADV(channel);
	  		delay(1);
	  }
	  delay(1);
}

void RF24BLE::sendADV(uint8_t channel){
	if (_length > 32){ Serial.print("ADV FAIL! Packet too Long"); return; }
	if(channel>2 ||channel<0){Serial.print("Incorrect channel value, use RF24BLE.advertise()"); return;}
	
	_radio.setChannel(RF24BLE::chRf[channel]);
	_packet[1] = _length-5;//subtract checksum bytes and the 2 bytes including the length byte and the first byte
	blePacketEncode(_packet, _length, RF24BLE::chLe[channel]);
	_radio.startWrite(_packet, _length,false);
#if DEBUG == 1
	Serial.print("final length "); Serial.println(_length);
#endif
}

void RF24BLE::printPacket(){
	for (uint8_t i = 0; i < _length; i++){
		Serial.print((char)_packet[i]);
	}
	Serial.println();

}
uint8_t RF24BLE::getPacketLengthCurr(){ return _length; }

uint8_t RF24BLE::recvPacket(uint8_t *input, uint8_t length,uint8_t channel ){
	unsigned long time = millis();
	while (_radio.available()<=0 && (millis()-time)<RECV_TIMEOUT){delay(1);}
	if (_radio.available()>0){
		_radio.read(input, length);
	}
	else { return RF24BLE_TIMEOUT; }
	uint8_t i, dataLen = length - 3;
#if DEBUG == 1
	for (i = 0; i < length; i++){
		Serial.print((char)input[i]);
	}Serial.println();
	// Packet length includes crc of 3 bytes
#endif
	//reversing the bits of the complete packet
	for (i = 0; i < length; i++){ input[i] = reverseBits(input[i]); }
	//de-whiten the packet using the same polynomial
	bleWhiten(input, length, bleWhitenStart(RF24BLE::chLe[channel]));
	//reversing bits of the crc 
	for (i = 0; i < 3; i++, dataLen++){ input[dataLen] = reverseBits(input[dataLen]); }
#if DEBUG == 1
	for (i = 0; i < length; i++){
		Serial.print((char)input[i]);
	}Serial.println();
	// Packet length includes crc of 3 bytes
#endif
	return checkCRC(input, length);	
}
