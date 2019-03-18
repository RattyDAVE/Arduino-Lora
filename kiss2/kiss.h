// this software is (C) 2016 by folkert@vanheusden.com
// AGPL v3.0

#include <stdint.h>

void calc_crc_flex(const uint8_t *cp, int size, uint16_t *const crc);

class kiss {
private:
	uint8_t *const bufferBig, *const bufferSmall;
	const uint16_t maxPacketSize;
	bool (* peekRadio)();
	void (* getRadio)(uint8_t *const whereTo, uint16_t *const n);
	void (* putRadio)(const uint8_t *const what, const uint16_t size);
	uint16_t (* peekSerial)();
	bool (* getSerial)(uint8_t *const whereTo, const uint16_t n, const unsigned long int to);
	void (* putSerial)(const uint8_t *const what, const uint16_t size);
	bool (* resetRadio)();
	uint8_t recvLedPin, sendLedPin, errorLedPin;

	void processRadio();
	void processSerial();
	void setError();

public:
	kiss(const uint16_t maxPacketSize, bool (* peekRadio)(), void (* getRadio)(uint8_t *const whereTo, uint16_t *const n), void (* putRadio)(const uint8_t *const what, const uint16_t size), uint16_t (*peekSerialIn)(), bool (* getSerialIn)(uint8_t *const whereTo, const uint16_t n, const unsigned long int to), void (*putSerialIn)(const uint8_t *const what, const uint16_t size), bool (* resetRadioIn)(), const uint8_t recvLedPin, const uint8_t sendLedPin, const uint8_t errorLedPin);
	~kiss();

	void debug(const char *const t);

	void begin();
	void loop();
};
