#include "utils.h"
#include <bcm2835.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_ECHO_WAITTIME 100000

// completes one pulse cycle to get the range, needs a trigger first
// this is for a single pin
long get_range(int echo_pin) {
  long start_time, end_time, range_cm, method_start_time;

  method_start_time = get_time_microsec();
  while (bcm2835_gpio_lev(echo_pin) == LOW) {
    start_time = get_time_microsec();
    if (start_time - method_start_time > MAX_ECHO_WAITTIME) {
      printf("No echo received..., %d pin failed.\n", echo_pin);
      return -1;
    }
  }
  while (bcm2835_gpio_lev(echo_pin) == HIGH) {
    end_time = get_time_microsec();
    if (end_time - start_time > MAX_ECHO_WAITTIME) {
      printf("Echo took too long to come back down..., %d pin failed.\n", echo_pin);
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

int main(void) {
  // init bcm2835
  if (!bcm2835_init())
    return 1;
  setup_ultrasonic_pins();

  pthread_t thread_id_1, thread_id_2, thread_id_3, thread_id_4, thread_id_5,
      thread_id_6, thread_id_7, thread_id_8;
  void *thread_ret_1, *thread_ret_2, *thread_ret_3, *thread_ret_4,
      *thread_ret_5, *thread_ret_6, *thread_ret_7, *thread_ret_8;

  while (1) {

    pulse_trigger(PIN_TRIGGER_1);

    pthread_create(&thread_id_1, NULL, threaded_range, (void *)PIN_ECHO_1);
    pthread_create(&thread_id_2, NULL, threaded_range, (void *)PIN_ECHO_2);
    pthread_create(&thread_id_3, NULL, threaded_range, (void *)PIN_ECHO_3);
    pthread_create(&thread_id_4, NULL, threaded_range, (void *)PIN_ECHO_4);
    pthread_join(thread_id_1, &thread_ret_1);
    pthread_join(thread_id_2, &thread_ret_2);
    pthread_join(thread_id_3, &thread_ret_3);
    pthread_join(thread_id_4, &thread_ret_4);

    //delay(1);
    pulse_trigger(PIN_TRIGGER_2);
    pthread_create(&thread_id_5, NULL, threaded_range, (void *)PIN_ECHO_5);
    pthread_create(&thread_id_6, NULL, threaded_range, (void *)PIN_ECHO_6);
    pthread_create(&thread_id_7, NULL, threaded_range, (void *)PIN_ECHO_7);
    pthread_create(&thread_id_8, NULL, threaded_range, (void *)PIN_ECHO_8);
    pthread_join(thread_id_5, &thread_ret_5);
    pthread_join(thread_id_6, &thread_ret_6);
    pthread_join(thread_id_7, &thread_ret_7);
    pthread_join(thread_id_8, &thread_ret_8);

    printf("Range 1: %ld\n", (long)thread_ret_1);
    printf("Range 2: %ld\n", (long)thread_ret_2);
    printf("Range 3: %ld\n", (long)thread_ret_3);
    printf("Range 4: %ld\n", (long)thread_ret_4);
    printf("Range 5: %ld\n", (long)thread_ret_5);
    printf("Range 6: %ld\n", (long)thread_ret_6);
    printf("Range 7: %ld\n", (long)thread_ret_7);
    printf("Range 8: %ld\n", (long)thread_ret_8);
    delay(20);
  }

  return 0;
}
