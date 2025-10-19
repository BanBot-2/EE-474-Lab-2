// Filename: Lab_2_Part_I_Step_1.ino
// Author: Yehoshua Luna
// Date: 10/17/2025
// Description: Makes an external LED blink on/off by
// directly selecting and modifying GPIO registers.

// ========== Includes ==========
#include "driver/gpio.h" // For GPIO driver-level macros
#include "soc/io_mux_reg.h" // For configuring the MUX (multiplexer)
#include "soc/gpio_reg.h" // For direct GPIO register access
#include "soc/gpio_periph.h" // For GPIO pin definitions

// =========== Constants ==========
#define LED_PIN 21

void setup() {
  // Selects LED_PIN as a general use GPIO pin.
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[LED_PIN], PIN_FUNC_GPIO);
  
  // Enables LED_PIN as an output pin.
  *((volatile uint32_t*)GPIO_ENABLE_REG) |= (1 << LED_PIN);
}

void loop() {
  // Toggles LED_PIN on/off using the XOR operator.
  *((volatile uint32_t *)GPIO_OUT_REG) ^= (1 << LED_PIN);
  delay(1000); // Waits 1 second
}