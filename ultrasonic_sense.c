#include "utils.h"
#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// completes one pulse cycle to get the range, needs a trigger first
// this is for a single pin
long get_range(int echo_pin) {
	long start_time, end_time, range_cm;
    while (bcm2835_gpio_lev(echo_pin) == LOW) {
    	start_time = get_time_microsec();
    }

    while (bcm2835_gpio_lev(echo_pin) == HIGH) {
    	end_time = get_time_microsec();
    }

    range_cm = (end_time - start_time) / 58;
    return range_cm;
}

void *threaded_range(void *echo_pin) {
	return (void*) get_range((int) echo_pin);
}

int main(void) {

    // init bcm2835
  if (!bcm2835_init())
    return 1;

  bcm2835_gpio_fsel(PIN_TRIGGER, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(PIN_TRIGGER, LOW);
  bcm2835_gpio_fsel(PIN_ECHO_1, BCM2835_GPIO_FSEL_INPT);
  bcm2835_gpio_fsel(PIN_ECHO_2, BCM2835_GPIO_FSEL_INPT);
  delay(500);


  while (1) {
	pthread_t thread_id_1, thread_id_2;
	void *thread_ret_1, *thread_ret_2;
	pulse_trigger();

	pthread_create(&thread_id_1, NULL, threaded_range, (void*)PIN_ECHO_1);
	pthread_create(&thread_id_2, NULL, threaded_range, (void*)PIN_ECHO_2);

	pthread_join(thread_id_1, &thread_ret_1);
	pthread_join(thread_id_2, &thread_ret_2);

    printf("Range 1: %ld\n", (long)thread_ret_1);
    printf("Range 2: %ld\n", (long)thread_ret_2);
  }

  return 0;
}
