// To compile: gcc -o sense main.c ultrasonic_sense.c utils.c
// resolution_getter.c -lbcm2835 -lpthread

#include "resolution_getter.h"
#include "ultrasonic_sense.h"
#include "utils.h"
#include <bcm2835.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// These are for scaling the mouse inputs. They are kind of like mouse
// sensitvity parameters.
#define MAX_THRESHOLD 50
#define MIN_THRESHOLD 5
#define ENABLE_BOUNDING 0

// Kalman Filter Parameters
// Code based off:
// https://towardsdatascience.com/a-simple-kalman-filter-implementation-e13f75987195
// Concepts learnt from: https://www.kalmanfilter.net/default.aspx
#define Q 0.5 // Transition Error ie. process noise
#define R 3   // Measurement Error
int predicted_y_prev = 0;
double p_prev_y = 1;

int kalman_filter_y(int measured_y) {
  // prediction
  double y_hat_k_a_priori = predicted_y_prev;
  double P_k_a_priori = p_prev_y + Q;

  // innovation
  double K_k = P_k_a_priori / (P_k_a_priori + R);
  double y_hat_k = y_hat_k_a_priori + K_k * (measured_y - y_hat_k_a_priori);
  double P_k = (1 - K_k) * P_k_a_priori;

  // save previous variables
  predicted_y_prev = (int)y_hat_k;
  p_prev_y = P_k;
  return y_hat_k;
}

#define Q_x 0.5 // Transition Error ie. process noise
#define R_x 5   // Measurement Error
int predicted_x_prev = 0;
double p_prev_x = 1;

int kalman_filter_x(int measured_x) {
  // prediction
  double x_hat_k_a_priori = predicted_x_prev;
  double P_k_a_priori = p_prev_x + Q_x;

  // innovation
  double K_k = P_k_a_priori / (P_k_a_priori + R_x);
  double x_hat_k = x_hat_k_a_priori + K_k * (measured_x - x_hat_k_a_priori);
  double P_k = (1 - K_k) * P_k_a_priori;

  // save previous variables
  predicted_x_prev = (int)x_hat_k;
  p_prev_x = P_k;
  return x_hat_k;
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
  for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
    x_bins[i] = i * x_bin_step;
  }

  pthread_t thread_id_1, thread_id_2, thread_id_3, thread_id_4, thread_id_5,
      thread_id_6, thread_id_7, thread_id_8;
  void *thread_ret_1, *thread_ret_2, *thread_ret_3, *thread_ret_4,
      *thread_ret_5, *thread_ret_6, *thread_ret_7, *thread_ret_8;

  int x_coordinate = 0;
  int y_coordinate = 0;
  int prev_x_coordinate = 0;
  int prev_y_coordinate = 0;

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
        (long)thread_ret_1, (long)thread_ret_2, (long)thread_ret_3,
        (long)thread_ret_4, (long)thread_ret_5, (long)thread_ret_6,
        (long)thread_ret_7, (long)thread_ret_8};

    // convert responses to mouse coordinates
    int position_sum = 0;
    int num_detected = 0;
    for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
      if (responses[i] < MAX_THRESHOLD && responses[i] >= MIN_THRESHOLD) {
        position_sum += i;
        num_detected++;
      }
    }

    if (num_detected != 0) {
      int predicted_x_bin = (int)(position_sum / num_detected);
      int sensor_y =
          res.yres - (int)(responses[predicted_x_bin] * y_scaling_factor);

      x_coordinate = kalman_filter_x(x_bins[predicted_x_bin]);
      y_coordinate = kalman_filter_y(sensor_y);

      if (responses[predicted_x_bin] < 0) {
        x_coordinate = prev_x_coordinate;
        y_coordinate = prev_y_coordinate;
      }

      if (ENABLE_BOUNDING) {
        if (abs(x_coordinate - prev_x_coordinate) > 200 &&
            prev_x_coordinate != 0) {
          x_coordinate = prev_x_coordinate;
        }

        if (abs(y_coordinate - prev_y_coordinate) > 200 &&
            prev_y_coordinate != 0) {
          y_coordinate = prev_y_coordinate;
        }
      }
    }

    // Publish mouse coordinates
    printf("Coordinates: (x=%d, y=%d)\n", x_coordinate, y_coordinate);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "xdotool mousemove %d %d", x_coordinate,
             y_coordinate);
    system(buffer);

    prev_x_coordinate = x_coordinate;
    prev_y_coordinate = y_coordinate;

    delay(20);
  }

  return 0;
}