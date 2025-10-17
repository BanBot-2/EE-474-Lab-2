// Filename: Lab_2_Part_II_Step_3.ino
// Author: Yehoshua Luna
// Date: 10/17/2025
// Description: Changes the brightness of an external LED
// using the analog input from a photoresistor and hardware PWM.

// ========== Includes ==========
#include "driver/gpio.h" // For GPIO driver-level macros
#include "soc/io_mux_reg.h" // For configuring the MUX (multiplexer)
#include "soc/gpio_reg.h" // For direct GPIO register access
#include "soc/gpio_periph.h" // For GPIO pin definitions

// =========== Constants ==========

// Defines LED and photoresistor macros.
#define LED_PIN 21
#define PHOTORES_PIN 2
#define INTERP_FACTOR 10000

void setup() {
  // Selects PHOTORES_PIN as a general use GPIO pin.
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[PHOTORES_PIN], PIN_FUNC_GPIO);

  // Enables PHOTORES_PIN as an input pin.
  *((volatile uint32_t*)GPIO_ENABLE_REG) |= (0 << PHOTORES_PIN);

  // Configures LED_PIN as a PWM output.
  ledcAttach(LED_PIN, 1000, 12);
}

void loop() {
  // Variables that track interpolation and final brightness.
  static float lerp = 0.0;
  static float brightness = 0.0;
  
  // Linearly interpolates to the analog PHOTORES reading.
  lerp += (analogRead(PHOTORES_PIN) - lerp) / INTERP_FACTOR;

  brightness = lerp * lerp / 4098; // Converts lerp to square scale

  // Sends PWM to LED_PIN with duty cycle equal to brightness.
  ledcWrite(LED_PIN, brightness);
}

