// Filename: Lab_2_Part_II_Step_3.ino
// Author: Yehoshua Luna
// Date: 10/17/2025
// Description: Makes an external LED blink on/off by directly selecting
// and modifying GPIO registers and one of the built-in hardware clocks.

// ========== Includes ==========
#include "driver/gpio.h" // For GPIO driver-level macros
#include "soc/io_mux_reg.h" // For configuring the MUX (multiplexer)
#include "soc/gpio_reg.h" // For direct GPIO register access
#include "soc/gpio_periph.h" // For GPIO pin definitions
#include "soc/timer_group_reg.h" // For timer functions

// =========== Constants ==========

// Defines LED related macros.
#define LED_PIN 21
#define TOGGLE_INTERVAL 1000000

// Defines timer configuration macros.
#define TIMER_INCREMENT_MODE (1 << 30)
#define TIMER_ENABLE (1 << 31)
#define TIMER_DIVIDER_VALUE 80

void setup() {
  // Selects LED_PIN as a general use GPIO pin.
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[LED_PIN], PIN_FUNC_GPIO);
  
  // Enables LED_PIN as an output pin.
  *((volatile uint32_t*)GPIO_ENABLE_REG) |= (1 << LED_PIN);

  // Creates a timer config variable and sets divider value.
  uint32_t timer_config = (TIMER_DIVIDER_VALUE << 13);

  // Sets increment mode and enable in timer configuration.
  timer_config |= TIMER_INCREMENT_MODE;
  timer_config |= TIMER_ENABLE;

  // Writes configuration to timer register.
  *((volatile uint32_t*)TIMG_T0CONFIG_REG(0)) = timer_config;

  // Triggers a timer update to load settings.
  *((volatile uint32_t *)TIMG_T0UPDATE_REG(0)) = 1;
}

void loop() {
  // Creates variables to track time.
  static uint32_t last_toggle_time = 0;
  static uint32_t current_time = 0;

  // Stores current time in variable through direct register access.
  current_time = *((volatile uint32_t *)TIMG_T0LO_REG(0));

  // Checks if the LED toggle interval has passed.
  if ((current_time - last_toggle_time) >= TOGGLE_INTERVAL) {
    // Toggles LED_PIN by directly modifying the GPIO_OUT register using XOR.
    *((volatile uint32_t *)GPIO_OUT_REG) ^= (1 << LED_PIN);

    // Updates last_toggle_time to current time.
    last_toggle_time = current_time;
  }

  // Refreshes timer so that time stays current.
  *((volatile uint32_t*)TIMG_T0UPDATE_REG(0)) = 1;
}

