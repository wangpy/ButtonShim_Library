#ifndef _BUTTONSHIM_h
#define _BUTTONSHIM_h

/***** Includes *****/
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Wire.h"

// Refer to: https://github.com/pimoroni/button-shim/
class ButtonShim {
 public:
  static const uint8_t DEFAULT_I2C_ADDRESS = 0x3F;

 private:
  static const uint8_t LED_DATA = 7;
  static const uint8_t LED_CLOCK = 6;

  // TCA9554A: https://www.ti.com/lit/ds/symlink/tca9554a.pdf
  static const uint8_t REG_INPUT = 0x00;
  static const uint8_t REG_OUTPUT = 0x01;
  static const uint8_t REG_POLARITY = 0x02;
  static const uint8_t REG_CONFIG = 0x03;

  static const uint8_t REG_QUEUE_SIZE = 32;

  static const uint8_t LED_GAMMA[256];

 public:
  ButtonShim(TwoWire& wire, uint8_t address = DEFAULT_I2C_ADDRESS);

  void initialise();
  void setRGB(uint8_t r, uint8_t g, uint8_t b);

  // return states of 5 buttons in bit 0-4, 0:pressed, 1:released
  uint8_t readButtonStates();

 private:
  uint8_t readByteData(uint8_t reg);
  void writeByteData(uint8_t reg, uint8_t value);

  void nextByteToLed();
  void writeQueuedBytesToLed();
  void setBitToLed(uint8_t pin, uint8_t value);
  void writeByteToLed(uint8_t value);

 private:
  TwoWire& _wire;
  uint8_t _address;
  uint8_t _reg_queue[REG_QUEUE_SIZE];
  int _reg_queue_data_len;
};

#endif /* BUTTONSHIM_H */
