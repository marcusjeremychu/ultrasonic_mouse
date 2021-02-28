#include <bcm2835.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include "utils.h"


long get_time_microsec() {
  struct timeval curr_time;
  gettimeofday(&curr_time, NULL);
  return (curr_time.tv_sec * (int)1e6) + curr_time.tv_usec;
}

void pulse_trigger() {
  bcm2835_gpio_write(PIN_TRIGGER, HIGH);
  bcm2835_delayMicroseconds(10);
  bcm2835_gpio_write(PIN_TRIGGER, LOW);
}
