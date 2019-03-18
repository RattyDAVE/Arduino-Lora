// this software is (C) 2016 by folkert@vanheusden.com
// AGPL v3.0

#ifdef UNITTEST
#include <string.h>
#include <time.h>
#include <unistd.h>
#else
#include <Arduino.h>
#endif
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "kiss.h"

unsigned long int ledOn = 0;

#ifdef UNITTEST
static const unsigned short crc_flex_table[] = {
#else
static const PROGMEM unsigned short crc_flex_table[] = {
#endif
        0x0f87, 0x1e0e, 0x2c95, 0x3d1c, 0x49a3, 0x582a, 0x6ab1, 0x7b38,
        0x83cf, 0x9246, 0xa0dd, 0xb154, 0xc5eb, 0xd462, 0xe6f9, 0xf770,
        0x1f06, 0x0e8f, 0x3c14, 0x2d9d, 0x5922, 0x48ab, 0x7a30, 0x6bb9,
        0x934e, 0x82c7, 0xb05c, 0xa1d5, 0xd56a, 0xc4e3, 0xf678, 0xe7f1,
        0x2e85, 0x3f0c, 0x0d97, 0x1c1e, 0x68a1, 0x7928, 0x4bb3, 0x5a3a,
        0xa2cd, 0xb344, 0x81df, 0x9056, 0xe4e9, 0xf560, 0xc7fb, 0xd672,
        0x3e04, 0x2f8d, 0x1d16, 0x0c9f, 0x7820, 0x69a9, 0x5b32, 0x4abb,
        0xb24c, 0xa3c5, 0x915e, 0x80d7, 0xf468, 0xe5e1, 0xd77a, 0xc6f3,
        0x4d83, 0x5c0a, 0x6e91, 0x7f18, 0x0ba7, 0x1a2e, 0x28b5, 0x393c,
        0xc1cb, 0xd042, 0xe2d9, 0xf350, 0x87ef, 0x9666, 0xa4fd, 0xb574,
        0x5d02, 0x4c8b, 0x7e10, 0x6f99, 0x1b26, 0x0aaf, 0x3834, 0x29bd,
        0xd14a, 0xc0c3, 0xf258, 0xe3d1, 0x976e, 0x86e7, 0xb47c, 0xa5f5,
        0x6c81, 0x7d08, 0x4f93, 0x5e1a, 0x2aa5, 0x3b2c, 0x09b7, 0x183e,
        0xe0c9, 0xf140, 0xc3db, 0xd252, 0xa6ed, 0xb764, 0x85ff, 0x9476,
        0x7c00, 0x6d89, 0x5f12, 0x4e9b, 0x3a24, 0x2bad, 0x1936, 0x08bf,
        0xf048, 0xe1c1, 0xd35a, 0xc2d3, 0xb66c, 0xa7e5, 0x957e, 0x84f7,
        0x8b8f, 0x9a06, 0xa89d, 0xb914, 0xcdab, 0xdc22, 0xeeb9, 0xff30,
        0x07c7, 0x164e, 0x24d5, 0x355c, 0x41e3, 0x506a, 0x62f1, 0x7378,
        0x9b0e, 0x8a87, 0xb81c, 0xa995, 0xdd2a, 0xcca3, 0xfe38, 0xefb1,
        0x1746, 0x06cf, 0x3454, 0x25dd, 0x5162, 0x40eb, 0x7270, 0x63f9,
        0xaa8d, 0xbb04, 0x899f, 0x9816, 0xeca9, 0xfd20, 0xcfbb, 0xde32,
        0x26c5, 0x374c, 0x05d7, 0x145e, 0x60e1, 0x7168, 0x43f3, 0x527a,
        0xba0c, 0xab85, 0x991e, 0x8897, 0xfc28, 0xeda1, 0xdf3a, 0xceb3,
        0x3644, 0x27cd, 0x1556, 0x04df, 0x7060, 0x61e9, 0x5372, 0x42fb,
        0xc98b, 0xd802, 0xea99, 0xfb10, 0x8faf, 0x9e26, 0xacbd, 0xbd34,
        0x45c3, 0x544a, 0x66d1, 0x7758, 0x03e7, 0x126e, 0x20f5, 0x317c,
        0xd90a, 0xc883, 0xfa18, 0xeb91, 0x9f2e, 0x8ea7, 0xbc3c, 0xadb5,
        0x5542, 0x44cb, 0x7650, 0x67d9, 0x1366, 0x02ef, 0x3074, 0x21fd,
        0xe889, 0xf900, 0xcb9b, 0xda12, 0xaead, 0xbf24, 0x8dbf, 0x9c36,
        0x64c1, 0x7548, 0x47d3, 0x565a, 0x22e5, 0x336c, 0x01f7, 0x107e,
        0xf808, 0xe981, 0xdb1a, 0xca93, 0xbe2c, 0xafa5, 0x9d3e, 0x8cb7,
        0x7440, 0x65c9, 0x5752, 0x46db, 0x3264, 0x23ed, 0x1176, 0x00ff
};

void calc_crc_flex(const uint8_t *cp, int size, uint16_t *const crc)
{
        while (size--) {
		uint16_t temp = ((*crc >> 8) ^ *cp++) & 0xff;

#ifdef UNITTEST
                *crc = (*crc << 8) ^ crc_flex_table[temp];
#else
                *crc = (*crc << 8) ^ pgm_read_word_near(crc_flex_table + temp);
#endif
	}
}

// note: it allocates 3 * maxPacketSize. so for lora devices that is 762 bytes of ram
// most arduinos have only 2048 bytes so that is quite a bit
// there's room for improvement here
kiss::kiss(const uint16_t maxPacketSizeIn, bool (* peekRadioIn)(), void (* getRadioIn)(uint8_t *const whereTo, uint16_t *const n), void (* putRadioIn)(const uint8_t *const what, const uint16_t size), uint16_t (*peekSerialIn)(), bool (* getSerialIn)(uint8_t *const whereTo, const uint16_t n, const unsigned long int to), void (*putSerialIn)(const uint8_t *const what, const uint16_t size), bool (* resetRadioIn)(), const uint8_t recvLedPinIn, const uint8_t sendLedPinIn, const uint8_t errorLedPinIn) : bufferBig(new uint8_t[maxPacketSizeIn * 2]), bufferSmall(new uint8_t[maxPacketSizeIn]), maxPacketSize(maxPacketSizeIn), peekRadio(peekRadioIn), getRadio(getRadioIn), putRadio(putRadioIn), peekSerial(peekSerialIn), getSerial(getSerialIn), putSerial(putSerialIn), resetRadio(resetRadioIn), recvLedPin(recvLedPinIn), sendLedPin(sendLedPinIn), errorLedPin(errorLedPinIn) {
	debug("START");
}

kiss::~kiss() {
	delete [] bufferSmall;
	delete [] bufferBig;
}

void kiss::begin() {
}

#define FEND	0xc0
#define FESC	0xdb
#define TFEND	0xdc
#define TFESC	0xdd

void put_byte(uint8_t *const out, uint16_t *const offset, const uint8_t c)
{
	if (c == FEND)
	{
		out[(*offset)++] = FESC;
		out[(*offset)++] = TFEND;
	}
	else if (c == FESC)
	{
		out[(*offset)++] = FESC;
		out[(*offset)++] = TFESC;
	}
	else
	{
		out[(*offset)++] = c;
	}
}

void kiss::setError() {
	if (errorLedPin != 0xff) {
#ifdef UNITTEST
		printf("setError\n");
#else
		digitalWrite(errorLedPin, HIGH);
		ledOn = millis();
#endif
	}
}

// FIXME combine with processRadio
// this test-method is used for debugging
// it sends some text packaged as a kiss-packet via
// the serial port to the linux system
void kiss::debug(const char *const t) {
#ifdef UNITTEST
	printf("DEBUG: %s\n", t);
#else
	uint8_t myBuffer[128];
	const uint8_t ax25_ident[] = { 0x92, 0x88, 0x8a, 0x9c, 0xa8, 0x40, 0xe0, 0x88, 0x8a, 0x84, 0xaa, 0x8e, 0x60, 0x63, 0x03, 0xf0 };

        uint16_t crc = 0xffff;

	uint16_t o = 0;
	myBuffer[o++] = 0x20; // FLEXNET

	for(uint8_t i=0; i<sizeof(ax25_ident); i++)
		myBuffer[o++] = ax25_ident[i];

	uint8_t l = strlen(t);
	for(uint8_t i=0; i<l; i++)
		myBuffer[o++] = t[i];

	calc_crc_flex(myBuffer, o, &crc);

	myBuffer[o++] = crc >> 8;
	myBuffer[o++] = crc & 0xff;

	const uint8_t fend = FEND;
	putSerial(&fend, 1);

	for(uint8_t i=0; i<o; i++) {
		uint8_t tinyBuffer[4];
		uint16_t oo = 0;
		put_byte(tinyBuffer, &oo, myBuffer[i]);
		putSerial(tinyBuffer, oo);
	}

	putSerial(&fend, 1);
#endif
}

void kiss::processRadio() {
	if (recvLedPin != 0xff) {
#ifndef UNITTEST
		digitalWrite(recvLedPin, HIGH);
		ledOn = millis();
#endif
	}

	uint16_t nBytes = maxPacketSize;
	getRadio(bufferSmall, &nBytes);

	char buf[20];
	snprintf(buf, sizeof buf, "recv radio %d", nBytes);
        debug(buf);

        uint16_t crc = 0xffff;

	uint16_t o = 0;
	bufferBig[o++] = FEND;

	bufferBig[o] = 0x20; // FLEXNET
	calc_crc_flex(&bufferBig[o++], 1, &crc);

	calc_crc_flex(bufferSmall, nBytes, &crc);

	for(uint16_t i=0; i<nBytes; i++)
		put_byte(bufferBig, &o, bufferSmall[i]);

	put_byte(bufferBig, &o, crc >> 8);
	put_byte(bufferBig, &o, crc & 0xff);

	bufferBig[o++] = FEND;

	putSerial(bufferBig, o);
}

// when a byte comes in via serial, it is expected that a full kiss
// packet comes in. this method waits for that with a timeout of
// 2 seconds
void kiss::processSerial() {
	if (sendLedPin != 0xff) {
#ifndef UNITTEST
		digitalWrite(sendLedPin, HIGH);
		ledOn = millis();
#endif
	}

	bool first = true, ok = false, escape = false;

	uint16_t o = 0;

#ifdef UNITTEST
	const time_t end = time(NULL) + 3;
	for(;time(NULL) < end;)
#else
	const unsigned long int end = millis() + 5000;
	for(;millis() < end;)
#endif
	{
		uint8_t buffer = 0;

		if (!getSerial(&buffer, 1, end)) {
			debug("ser recv to");
			setError();
			break;
		}

		if (escape)
		{
			if (o == maxPacketSize) {
				debug("error packet size2");
				setError();
				break;
			}

			if (buffer == TFEND)
				bufferSmall[o++] = FEND;
			else if (buffer == TFESC)
				bufferSmall[o++] = FESC;
			else {
				debug("error escape");
				setError();
			}

			escape = false;
		}
		else if (buffer == FEND)
		{
			if (first) {
				first = false;

				if (o) {
					debug("ign garbage");
					o = 0;
				}
			}
			else
			{
				ok = true;
				break;
			}
		}
		else if (buffer == FESC)
			escape = true;
		else
		{
			if (o == maxPacketSize) {
				debug("error packet size3");
				setError();
				break;
			}

			bufferSmall[o++] = buffer;
		}
	}

	if (ok)
	{
		uint8_t cmd = bufferSmall[0] & 0x0f;

		if (cmd == 0x00 && o >= 2) // data frame
		{
			if (bufferSmall[0] & 0x20)
			{
				uint16_t crc_calc = 0xffff;
				calc_crc_flex(bufferSmall, o - 2, &crc_calc);
				unsigned short crc_recv = (bufferSmall[o - 2] << 8) | bufferSmall[o - 1];

				if (crc_calc != crc_recv)
				{
					debug("error crc");
					setError();
					ok = false;
				}
			}
			else
			{
				debug("error !0x20");
				setError();
				ok = false;
			}

			if (bufferSmall[0]) {
				if (o < 3) {
					debug("error < 3");
					ok = false;
					setError();
				}
				else {
					o -= 3;
				}
			}
			else {
				if (o < 1) {
					debug("error < 1");
					ok = false;
					setError();
				}
				else {
					o -= 1;
				}
			}

			if (o > 0)
				putRadio(&bufferSmall[1], o);
		}
		else if ((bufferSmall[0] & 0x0f) == 0x0f) {
			if (!resetRadio()) {
				debug("Reset radio failed");
				ok = false;

#ifndef UNITTEST
				for(byte i=0; i<3; i++) {
					digitalWrite(errorLedPin, HIGH);
					delay(250);
					digitalWrite(errorLedPin, LOW);
					delay(250);
				}
#endif
			}
		}
		else
		{
			char err[64];
			snprintf(err, sizeof err, "frame type %02x unk", bufferSmall[0]);
			debug(err);
			ok = false;
		}
	}


	static uint16_t cnt = 0;
	char buf[16];

	if (ok)
		snprintf(buf, sizeof buf, "OK %d", ++cnt);
	else
		snprintf(buf, sizeof buf, "FAIL %d", ++cnt);

	buf[sizeof(buf) - 1] = 0x00;

	debug(buf);
}

void kiss::loop() {
	if (peekRadio())
		processRadio();

	bool serialIn = peekSerial();
	if (serialIn)
		processSerial();

#ifndef UNITTEST
	const unsigned long int now = millis();
	if (ledOn && now - ledOn > 100) {
		digitalWrite(errorLedPin, LOW);
		digitalWrite(recvLedPin, LOW);
		digitalWrite(sendLedPin, LOW);
		ledOn = 0;
	}
#endif
}

#ifdef UNITTEST
const char radioStr[] = "Dit is een test.";
uint16_t peekRadio() {
	static bool state = true;
	int rc = state ? strlen(radioStr) : 0;
	state = false;
	return rc;
}

void getRadio(uint8_t *const whereTo, uint16_t *const n) {
	if (*n != strlen(radioStr))
		printf("getRadio: wrong number (get: %d, have: %d)\n", *n, strlen(radioStr));
	memcpy(whereTo, radioStr, *n);
}

void putRadio(const uint8_t *const what, const uint16_t size) {
	printf("put radio: ");
	for(int i=0; i<size; i++)
		printf("%c [%02x] ", what[i] > 32 && what[i] < 127 ? what[i] : '.', what[i]);
	printf("\n");
}

static int offset = 0;
const uint8_t serialData[] = { 0xc0, 0x20, 0x44, 0x69, 0x74, 0x20, 0x69, 0x73, 0x20, 0x65, 0x65, 0x6e, 0x20, 0x74, 0x65, 0x73, 0x74, 0x2e, 0x48, 0xc1, 0xc0 };
uint16_t peekSerial() {
	return sizeof serialData - offset;
}

void getSerial(uint8_t *const whereTo, const uint16_t n, const unsigned long int to) {
	if (offset > sizeof serialData)
		printf("getSerial: overflwo\n");

	memcpy(whereTo, &serialData[offset], n);
	offset += n;
}

void putSerial(const uint8_t *const what, const uint16_t size) {
	printf("put serial: ");
	for(int i=0; i<size; i++)
		printf("%c [%02x] ", what[i] > 32 && what[i] < 127 ? what[i] : '.', what[i]);
	printf("\n");
}

bool resetRadio() {
	printf("resetRadio\n");

	return true;
}

int main(int argc, char *argv[])
{
	kiss k(254, peekRadio, getRadio, putRadio, peekSerial, getSerial, putSerial, resetRadio, 1, 2, 3);
	k.begin();

	for(int i=0; i<5; i++) {
		printf("hb\n");
		k.loop();
		usleep(251000);
	}

	return 0;
}
#endif
