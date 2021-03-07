#define MAX_ECHO_WAITTIME 100000
#define NUMBER_OF_SENSORS 8

long get_range(int echo_pin);
void *threaded_range(void *echo_pin);
void setup_ultrasonic_pins();