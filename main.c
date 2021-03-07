// To compile: gcc -o sense main.c ultrasonic_sense.c utils.c resolution_getter.c -lbcm2835 -lpthread

#include "utils.h"
#include "ultrasonic_sense.h"
#include "resolution_getter.h"
#include <bcm2835.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// These are for scaling the mouse inputs. They are kind of like mouse sensitvity parameters.
#define MAX_THRESHOLD 50
#define MIN_THRESHOLD 7

#define Q 0.00001
#define R 0.003

int y_hat_k_minus_1;
int P_k_minus_1;

int kalman_filter(int measured_y){
    //prediction
    int y_hat_k_a_priori = y_hat_k_minus_1;
    int P_k_a_priori = P_k_minus_1 + Q;
    
    //innovation 
    int K_k = P_k_a_priori * (P_k_a_priori + R);
    int y_hat_k = y_hat_k_a_priori + K_k * (measured_y - y_hat_k_a_priori);
    int P_k = (1 - K_k) * P_k_a_priori;
    
    // save previous variables
    y_hat_k_minus_1 = y_hat_k;
    P_k_minus_1 = P_k;
    return y_hat_k;
}

int main(void) {

  // Setup and initialization
  if (!bcm2835_init())
    return 1;
  setup_ultrasonic_pins();
  struct resolution res = get_resolution();

  // setup coordinate system to map sensor inputs to mouse inputs
  double y_scaling_factor = res.yres / MAX_THRESHOLD;
  int x_bins[NUMBER_OF_SENSORS];
  int x_bin_step = res.xres / NUMBER_OF_SENSORS;
  for (int i = 0 ; i < NUMBER_OF_SENSORS; i++) {
    x_bins[i] = i * x_bin_step;
  }
  
  pthread_t thread_id_1, thread_id_2, thread_id_3, thread_id_4, 
            thread_id_5, thread_id_6, thread_id_7, thread_id_8;
  void *thread_ret_1, *thread_ret_2, *thread_ret_3, *thread_ret_4,
       *thread_ret_5, *thread_ret_6, *thread_ret_7, *thread_ret_8;

  int x_coordinate = 0;
  int y_coordinate = 0;
  y_hat_k_minus_1 = 0;
  P_k_minus_1 = 0;

  // main control loop
  while (1) {
    // Sensors range from 1 - 4
    pulse_trigger(PIN_TRIGGER_1);
    pthread_create(&thread_id_1, NULL, threaded_range, (void *)PIN_ECHO_1);
    pthread_create(&thread_id_2, NULL, threaded_range, (void *)PIN_ECHO_2);
    pthread_create(&thread_id_3, NULL, threaded_range, (void *)PIN_ECHO_3);
    pthread_create(&thread_id_4, NULL, threaded_range, (void *)PIN_ECHO_4);
    pthread_join(thread_id_1, &thread_ret_1);
    pthread_join(thread_id_2, &thread_ret_2);
    pthread_join(thread_id_3, &thread_ret_3);
    pthread_join(thread_id_4, &thread_ret_4);

    // Sensors range from 5 - 8
    pulse_trigger(PIN_TRIGGER_2);
    pthread_create(&thread_id_5, NULL, threaded_range, (void *)PIN_ECHO_5);
    pthread_create(&thread_id_6, NULL, threaded_range, (void *)PIN_ECHO_6);
    pthread_create(&thread_id_7, NULL, threaded_range, (void *)PIN_ECHO_7);
    pthread_create(&thread_id_8, NULL, threaded_range, (void *)PIN_ECHO_8);
    pthread_join(thread_id_5, &thread_ret_5);
    pthread_join(thread_id_6, &thread_ret_6);
    pthread_join(thread_id_7, &thread_ret_7);
    pthread_join(thread_id_8, &thread_ret_8);

    // save responses
    long responses[NUMBER_OF_SENSORS] = { 
      (long)thread_ret_1, (long)thread_ret_2, (long)thread_ret_3, (long)thread_ret_4,
      (long)thread_ret_5, (long)thread_ret_6, (long)thread_ret_7, (long)thread_ret_8
    };

    // convert responses to mouse coordinates
    int position_sum = 0;
    int num_detected = 0;
    for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
      if (responses[i] < y_scaling_factor && responses[i] >= MIN_THRESHOLD) {
        position_sum += i;
        num_detected++;
      }
    }
    
    if (num_detected != 0) {
      int predicted_x_bin = (int) (position_sum/num_detected);
      x_coordinate = x_bins[predicted_x_bin];
      int sensor_y = res.yres - (int)(responses[predicted_x_bin] * y_scaling_factor);
      y_coordinate = kalman_filter(sensor_y);
    }

    printf("Coordinates: (x=%d, y=%d)\n", x_coordinate, y_coordinate);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "xdotool mousemove %d %d", x_coordinate, y_coordinate);
    system(buffer);
    
    // printf("=============================================\n");
    // printf("1     2     3     4     5     6     7     8\n");
    // printf("%d,  %d,  %d,  %d,  %d,  %d,  %d,  %d\n", 
    //       one_hot_x[0], one_hot_x[1], one_hot_x[2], one_hot_x[3],
    //       one_hot_x[4], one_hot_x[5], one_hot_x[6], one_hot_x[7]);
    // printf("=============================================\n");

    // printf("=============================================\n");
    // printf("1     2     3     4     5     6     7     8\n");
    // printf("%ld,  %ld,  %ld,  %ld,  %ld,  %ld,  %ld,  %ld\n", 
    //       (long)thread_ret_1, (long)thread_ret_2, (long)thread_ret_3, (long)thread_ret_4,
    //       (long)thread_ret_5, (long)thread_ret_6, (long)thread_ret_7, (long)thread_ret_8);
    // printf("=============================================\n");
    
    delay(20);
  }

  return 0;
}