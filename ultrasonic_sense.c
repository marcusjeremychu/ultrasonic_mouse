#include "ultrasonic_sense.h"
#include "utils.h"
#include <bcm2835.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// completes one pulse cycle to get the range, needs a trigger first
// this is for a single pin
long get_range(int echo_pin) {
  long start_time, end_time, range_cm, method_start_time;

  method_start_time = get_time_microsec();
  while (bcm2835_gpio_lev(echo_pin) == LOW) {
    start_time = get_time_microsec();
    if (start_time - method_start_time > MAX_ECHO_WAITTIME) {
      printf("No echo received..., pin %d failed.\n", echo_pin);
      return -1;
    }
  }
  while (bcm2835_gpio_lev(echo_pin) == HIGH) {
    end_time = get_time_microsec();
    if (end_time - start_time > MAX_ECHO_WAITTIME) {
      printf("Echo took too long to come back down..., %d pin failed.\n",
             echo_pin);
      return -1;
    }
  }

  range_cm = (end_time - start_time) / 58.0;
  if (range_cm < 3 || range_cm > 300) {
    return -1;
  } else {
    return range_cm;
  }
}

void *threaded_range(void *echo_pin) {
  return (void *)get_range((int)echo_pin);
}

void setup_ultrasonic_pins() {
  // setup trigger pin
  bcm2835_gpio_fsel(PIN_TRIGGER_1, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_fsel(PIN_TRIGGER_2, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(PIN_TRIGGER_1, LOW);
  bcm2835_gpio_write(PIN_TRIGGER_2, LOW);

  // setup echo pins
  bcm2835_gpio_fsel(PIN_ECHO_1, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(PIN_ECHO_2, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(PIN_ECHO_3, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(PIN_ECHO_4, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(PIN_ECHO_5, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(PIN_ECHO_6, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(PIN_ECHO_7, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(PIN_ECHO_8, BCM2835_GPIO_FSEL_INPT);
  delay(500);
}