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

// =========== Constants ==========

// Defines photoresistor and piezzo macros.
#define PHOTORES_PIN 2
#define PIEZZO_PIN 47

// Defines ambient threshold cutoff.
#define LIGHT_THRESH 1500

// Defines frequency step, resolution, and buzz time.
#define BUZZ_RES 12
#define BUZZ_TIME 500000
#define BUZZ_MAX_FREQ 1000
#define BASE_BUZZ_FREQ 100
#define BUZZ_MULT BASE_BUZZ_FREQ / BUZZ_MAX_FREQ * pow(2, BUZZ_RES)

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
  ledcAttach(PIEZZO_PIN, BUZZ_MAX_FREQ, BUZZ_RES);

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
  // Creates variables to track time.
  static uint32_t threshold_time = 0;
  static uint32_t current_time = 0;

  // Creates variables to track piezzo signal.
  static uint32_t freq_order = 0;
  static uint32_t duty_cycle = 0;

  Serial.begin(115200);

  // Stores current time in variable through direct register access.
  current_time = *((volatile uint32_t *)TIMG_T0LO_REG(0));

  if (analogRead(PHOTORES_PIN) < LIGHT_THRESH) {
    freq_order = ceil((current_time - threshold_time) / BUZZ_TIME);
    duty_cycle = freq_order * BUZZ_MULT;
    Serial.println(duty_cycle);
  } else {
    threshold_time = current_time;
    duty_cycle = 0;
  }

  ledcWrite(PIEZZO_PIN, duty_cycle);

  // Refreshes timer so that time stays current.
  *((volatile uint32_t*)TIMG_T0UPDATE_REG(0)) = 1;
}
