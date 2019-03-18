// this software is (C) 2016 by folkert@vanheusden.com
// AGPL v3.0

#include "kiss.h"

#include <SPI.h>
#include <RH_RF95.h> // LoRa device

// as debugging via serial is not possible (it is used for the kiss
// protocol), I use a couple of LEDs
#define pinLedError 3
#define pinLedRecv 4
#define pinLedSend 5
#define pinLedHB 6
#define pinReset 7

// this is an example implementation using a "RadioHead"-driver for
// RF95 radio (a LoRa device).
// RadioHead: https://github.com/PaulStoffregen/RadioHead.git
static RH_RF95 rf95;

// the 'kiss'-class requires a couple of callback functions. these
// functions do the low-level work so that the kiss-class is generic

// a call-back function which you can adjust into something that
// peeks in the radio-buffer if anything is waiting
bool peekRadio() {
	return rf95.available();
}

// if there's data in your radio, then this callback should retrieve it
void getRadio(uint8_t *const whereTo, uint16_t *const n) {
	uint8_t dummy = *n;
	rf95.recv(whereTo, &dummy);
	*n = dummy;
}

void putRadio(const uint8_t *const what, const uint16_t size) {
	rf95.send(what, size);
	rf95.waitPacketSent();
}

// some arduino-platforms (teensy, mega) have multiple serial ports
// there you need to replace Serial by e.g. Serial2 or so
uint16_t peekSerial() {
	return Serial.available();
}

bool getSerial(uint8_t *const whereTo, const uint16_t n, const unsigned long int to) {
	for(uint16_t i=0; i<n; i++) {
		while(!Serial.available()) {
			if (millis() >= to)
				return false;
		}

		whereTo[i] = Serial.read();
	}

	return true;
}

void putSerial(const uint8_t *const what, const uint16_t size) {
	Serial.write(what, size);
}

bool initRadio() {
	if (rf95.init()) {
		delay(100);

		digitalWrite(pinLedRecv, LOW);
		digitalWrite(pinLedSend, LOW);
		digitalWrite(pinLedError, LOW);
		digitalWrite(pinLedHB, LOW);

#if 1
                const RH_RF95::ModemConfig cfg = {
                                // Register 0x1D:
                                // BW         CR      0=explicit
                                (8 << 4) | (4 << 1) | (0 << 0),
                                // Register 0x1E:
                                // SF       CRC enable
                                (10 << 4) | (1 << 2),
                                // Register 0x26:
                                // bit3 = LowDataRateOptimization
                                (0 << 3)
                        };
                rf95.setModemRegisters(&cfg);
                rf95.setFrequency(869.850);
                rf95.setPreambleLength(8);
#else
                rf95.setFrequency(869.525);
                rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
                rf95.setTxPower(13); // radiohead default is 13
#endif

		return true;
	}

	return false;
}

bool resetRadio() {
	digitalWrite(pinReset, LOW);
	delay(1); // at least 100us, this is 1000us
	digitalWrite(pinReset, HIGH);
	delay(5 + 1); // 5ms is required

	return initRadio();
}

// LoRa device can have a packetsize of 254 bytes
kiss k(254, peekRadio, getRadio, putRadio, peekSerial, getSerial, putSerial, resetRadio, pinLedRecv, pinLedSend, pinLedError);

void setup() {
	// the arduino talks with 9600bps to the linux system
	Serial.begin(9600);

	pinMode(pinLedRecv, OUTPUT);
	digitalWrite(pinLedRecv, HIGH);
	pinMode(pinLedSend, OUTPUT);
	digitalWrite(pinLedSend, HIGH);
	pinMode(pinLedError, OUTPUT);
	digitalWrite(pinLedError, HIGH);
	pinMode(pinLedHB, OUTPUT);
	digitalWrite(pinLedHB, HIGH);

	pinMode(pinReset, OUTPUT);
	digitalWrite(pinReset, HIGH);

	k.begin();

	if (!resetRadio())
		k.debug("Radio init failed");

	k.debug("Go!");
}

void loop() {
	k.loop();

	const unsigned long int now = millis();
	static unsigned long int pHB = 0;

	if (now - pHB >= 500) {
		static bool state = true;
		digitalWrite(pinLedHB, state ? HIGH : LOW);
		state = !state;
		pHB = now;
	}

	static unsigned long int lastReset = 0;
	const unsigned long int resetInterval = 301000; // every 5 min
	if (now - lastReset >= resetInterval) {
		k.debug("Reset radio");

		if (!resetRadio()) {
			for(byte i=0; i<3; i++) {
				digitalWrite(pinLedError, HIGH);
				delay(250);
				digitalWrite(pinLedError, LOW);
				delay(250);
			}
		}

		lastReset = now;
	}
}
