/*
 * ButtonShim example
 *
 * This example set color of LED by button presses.
 *
 * by Brian Ping-Yao Wang (@wangpy)
 */

#include <Wire.h>
#include <ButtonShim.h>

const uint8_t COLORS[5][3] = {
  255, 0, 0,
  0, 255, 0,
  0, 0, 255,
  255, 255, 0,
  255, 0, 255
};

ButtonShim buttonShim(Wire);

void setup() {
  buttonShim.initialise();
}

void loop() {
  uint8_t button_states = buttonShim.readButtonStates();
  bool pressed = false;
  for (uint8_t i=0; i<5; i++) {
    if ((button_states & (1 << i)) == 0) {
      // button i is pressed
      buttonShim.setRGB(COLORS[i][0], COLORS[i][1], COLORS[i][2]);
      pressed = true;
      break;
    }
  }
  if (!pressed) {
    buttonShim.setRGB(0, 0, 0);
  }

  delay(100);
}
