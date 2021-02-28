#include "utils.h"
#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {

    // init bcm2835
  if (!bcm2835_init())
    return 1;

    // set pins to respective functions
  printf("Attempting to fsel pins...\n");
  bcm2835_gpio_fsel(PIN_TRIGGER, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(PIN_TRIGGER, LOW);
  bcm2835_gpio_fsel(PIN_ECHO_1, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(PIN_ECHO_2, BCM2835_GPIO_FSEL_INPT);

  long start_time, end_time, range_cm;
  int max = 80, check;
  printf("Initialized, starting while loop\n");
  delay(500);

  while (1) {
    pulse_trigger();

    // I might need to make a separate thread for each echo pin to track
    // length of the echo pulse
    while (bcm2835_gpio_lev(PIN_ECHO_1) == LOW) {
    	start_time = get_time_microsec();
    }

    while (bcm2835_gpio_lev(PIN_ECHO_1) == HIGH) {
    	end_time = get_time_microsec();
    }
    //("Start_time: %ld, ", start_time);
    //printf("End_time: %ld\n", end_time);

    range_cm = (end_time - start_time) / 58;
    printf("Range: %ld\n", range_cm);
  }

  return 0;
}
