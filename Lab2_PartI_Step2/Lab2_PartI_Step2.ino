// Filename: Lab_2_Part_I_Step_2.ino
// Author: Yehoshua Luna
// Date: 10/17/2025
// Description: Compares timing between digitalWrite() and direct register
// access for toggling an external LED. Execution speeds timed using micros().

// ========== Includes ==========
#include "driver/gpio.h" // For GPIO driver-level macros
#include "soc/io_mux_reg.h" // For configuring the MUX (multiplexer)
#include "soc/gpio_reg.h" // For direct GPIO register access
#include "soc/gpio_periph.h" // For GPIO pin definitions

// =========== Constants ==========
#define LED_PIN 21
#define NUM_TOGGLES 1000

void setup() {
  Serial.begin(115200); // Opens serial connection

  // Selects LED_PIN as a general use GPIO pin.
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[LED_PIN], PIN_FUNC_GPIO);

  // Enables LED_PIN as an output pin.
  *((volatile uint32_t*)GPIO_ENABLE_REG) |= (1 << LED_PIN);
}

void loop() {
  unsigned long start, end, elapsed;

  // ========== digitalWrite() ==========
  start = micros();

  // Toggles LED_PIN on and off NUM_TOGGLES times using digitalWrite().
  for (int i = 0; i < NUM_TOGGLES; i++) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
  }

  end = micros();
  elapsed = end-start; // Calculated elapsed time

  // Prints the elapsed time during toggling to the serial connection.
  Serial.printf("digitalWrite() time for %d toggles: %lu µs\n", NUM_TOGGLES, elapsed);

  delay(1000); // Pause between tests

  // ========== Direct Register Access ==========
  start = micros();

  // Toggles LED_PIN on and off NUM_TOGGLES times using direct register modification.
  for (int i = 0; i < NUM_TOGGLES * 2; i++) {
    *((volatile uint32_t *)GPIO_OUT_REG) ^= (1 << LED_PIN);
  }

  end = micros();
  elapsed = end-start; // Calculated elapsed time

  // Prints the elapsed time during toggling to the serial connection.
  Serial.printf("Direct register modification time for %d toggles: %lu µs\n", NUM_TOGGLES, elapsed);
}