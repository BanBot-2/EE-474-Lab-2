// Filename: Lab_2_Part_IV_Step_5.ino
// Author: Yehoshua Luna
// Date: 10/17/2025
// Description: Brightness alarm that plays a series of frequencies
// when the ambient brightness drops below an arbitrary threshold.

// ========== Includes ==========
#include "driver/gpio.h" // For GPIO driver-level macros
#include "soc/io_mux_reg.h" // For configuring the MUX (multiplexer)
#include "soc/gpio_reg.h" // For direct GPIO register access
#include "soc/gpio_periph.h" // For GPIO pin definitions
#include "soc/timer_group_reg.h" // For timer functions
#include "driver/ledc.h" // For ledc functions

// =========== Constants ==========

// Defines photoresistor and piezo macros.
#define PHOTORES_PIN 2
#define PIEZZO_PIN 47

#define LIGHT_THRESH 1500 // Ambient light threshold cutoff

#define BUZZ_RES 12 // PWM resolution for piezo
#define BUZZ_TIME 500000.0 // Time per frequency in microseconds. Keep floating-point for calculations!
#define BUZZ_FREQ 500 // Base frequency multiple
#define BUZZ_SERIES 3 // How many increments of BUZZ_FREQ tone pattern can have before stopping

// Defines timer configuration macros.
#define TIMER_INCREMENT_MODE (1 << 30)
#define TIMER_ENABLE (1 << 31)
#define TIMER_DIV_VALUE 80

void setup() {
  // Selects PHOTORES_PIN as a general use GPIO pin.
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[PHOTORES_PIN], PIN_FUNC_GPIO);

  // Enables PHOTORES_PIN as an input pin.
  *((volatile uint32_t*)GPIO_ENABLE_REG) |= (0 << PHOTORES_PIN);

  // Configures LED_PIN as a PWM output.
  ledcAttach(PIEZZO_PIN, BUZZ_FREQ, BUZZ_RES);

  // Creates a timer config variable and sets divider value.
  uint32_t timer_config = (TIMER_DIV_VALUE << 13);

  // Sets increment mode and enable in timer configuration.
  timer_config |= TIMER_INCREMENT_MODE;
  timer_config |= TIMER_ENABLE;

  // Writes configuration to timer register.
  *((volatile uint32_t*)TIMG_T0CONFIG_REG(0)) = timer_config;

  // Triggers a timer update to load settings.
  *((volatile uint32_t *)TIMG_T0UPDATE_REG(0)) = 1;
}

void loop() {
  // Creates variables to track time and piezo signal.
  static uint32_t threshold_time = 0;
  static uint32_t current_time = 0;
  static uint32_t freq_order = 0;
  static uint32_t last_order = 0;
  static uint32_t duty_cycle = 0;

  // Stores current time in variable through direct register access.
  current_time = *((volatile uint32_t *)TIMG_T0LO_REG(0));

  if (analogRead(PHOTORES_PIN) < LIGHT_THRESH) { // Checks if light is below threshold
    // Every BUZZ_TIME microseconds since threshold, increase frequency by BUZZ_FREQ.
    freq_order = ceil((current_time - threshold_time) / BUZZ_TIME) * BUZZ_FREQ;
    
    // Only updates piezo frequency when frequency changes and frequency isn't too high.
    if (freq_order != last_order & freq_order <= BUZZ_FREQ * BUZZ_SERIES) {
      ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq_order); // Sets new frequency
    }

    // Notes any new changes in frequency and sets 50% duty cycle.
    last_order = freq_order;
    duty_cycle = pow(2, BUZZ_RES) / 2;
  } else {
    // Actively resets time since threshold trigger and zeros duty_cycle.
    threshold_time = current_time;
    last_order = 0;
    duty_cycle = 0;
  }

  ledcWrite(PIEZZO_PIN, duty_cycle);

  // Refreshes timer so that time stays current.
  *((volatile uint32_t*)TIMG_T0UPDATE_REG(0)) = 1;
}
