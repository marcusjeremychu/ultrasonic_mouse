
#define PIN_ECHO_1 RPI_GPIO_P1_16   // GPIO 23
#define PIN_ECHO_2 RPI_GPIO_P1_18  // GPIO 24
#define PIN_TRIGGER RPI_GPIO_P1_12  // GPIO 18 PCM_CLK

long get_time_microsec();
void pulse_trigger();
