#include "ButtonShim.h"

const uint8_t ButtonShim::LED_GAMMA[256] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
  2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
  6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11,
  11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18,
  19, 19, 20, 21, 21, 22, 22, 23, 23, 24, 25, 25, 26, 27, 27, 28,
  29, 29, 30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 37, 38, 39, 40,
  40, 41, 42, 43, 44, 45, 46, 46, 47, 48, 49, 50, 51, 52, 53, 54,
  55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
  71, 72, 73, 74, 76, 77, 78, 79, 80, 81, 83, 84, 85, 86, 88, 89,
  90, 91, 93, 94, 95, 96, 98, 99, 100, 102, 103, 104, 106, 107, 109, 110,
  111, 113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 128, 129, 131, 132, 134,
  135, 137, 138, 140, 142, 143, 145, 146, 148, 150, 151, 153, 155, 157, 158, 160,
  162, 163, 165, 167, 169, 170, 172, 174, 176, 178, 179, 181, 183, 185, 187, 189,
  191, 193, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220,
  222, 224, 227, 229, 231, 233, 235, 237, 239, 241, 244, 246, 248, 250, 252, 255};

ButtonShim::ButtonShim(TwoWire& wire, uint8_t address)
  : _wire(wire)
  , _address(address)
  , _reg_queue_data_len(0) {
}

void ButtonShim::initialise() {
  _wire.begin();

  writeByteData(REG_CONFIG, 0b00011111);
  writeByteData(REG_POLARITY, 0b00000000);
  writeByteData(REG_OUTPUT, 0b00000000);

  setRGB(0, 0, 0);
}

void ButtonShim::setRGB(uint8_t r, uint8_t g, uint8_t b) {
  writeByteToLed(0);
  writeByteToLed(0);
  writeByteToLed(0b11101111);
  writeByteToLed(LED_GAMMA[b]);
  writeByteToLed(LED_GAMMA[g]);
  writeByteToLed(LED_GAMMA[r]);
  writeByteToLed(0);
  writeByteToLed(0);
  writeQueuedBytesToLed();
}

uint8_t ButtonShim::readButtonStates() {
  return readByteData(REG_INPUT) & 0b00011111;
}

uint8_t ButtonShim::readByteData(uint8_t reg) {
  _wire.beginTransmission(_address);
  _wire.write(reg);
  _wire.endTransmission();
  _wire.requestFrom((int)_address, 1);
  uint8_t value = _wire.read();
  return value;
}

void ButtonShim::writeByteData(uint8_t reg, uint8_t value) {
  _wire.beginTransmission(_address);
  _wire.write(reg);
  _wire.write(value);
  _wire.endTransmission();
}

void ButtonShim::nextByteToLed() {
  if (_reg_queue_data_len == REG_QUEUE_SIZE) {
    writeQueuedBytesToLed();
  }
  if (_reg_queue_data_len > 0) {
    _reg_queue[_reg_queue_data_len] = _reg_queue[_reg_queue_data_len-1];
  } else {
    _reg_queue[0] = 0b00000000;
  }
  _reg_queue_data_len++;
}

void ButtonShim::writeQueuedBytesToLed() {
  if (_reg_queue_data_len == 0) {
    return;
  }

  _wire.beginTransmission(_address);
  for (uint8_t i=0; i<_reg_queue_data_len; i++) {
    writeByteData(REG_OUTPUT, _reg_queue[i]);
  }
  _wire.endTransmission();
  _reg_queue_data_len = 0;
}

void ButtonShim::setBitToLed(uint8_t pin, uint8_t value) {
  if (_reg_queue_data_len == 0) {
    return;
  }
  if (value) {
    _reg_queue[_reg_queue_data_len - 1] |= (1 << pin);
  } else {
    _reg_queue[_reg_queue_data_len - 1] &= 0xFF ^ (1 << pin);
  }
}

void ButtonShim::writeByteToLed(uint8_t value) {
  for (uint8_t i=0; i<8; i++) {
    nextByteToLed();
    setBitToLed(LED_CLOCK, 0);
    setBitToLed(LED_DATA, value & 0b10000000);
    nextByteToLed();
    setBitToLed(LED_CLOCK, 1);
    value <<= 1;
  }
}
