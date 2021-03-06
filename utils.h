#define PIN_TRIGGER_1 RPI_GPIO_P1_12  // GPIO 18 PCM_CLK
#define PIN_TRIGGER_2 RPI_V2_GPIO_P1_37 //GPIO 26

#define PIN_ECHO_1 RPI_GPIO_P1_16   // GPIO 23
#define PIN_ECHO_2 RPI_GPIO_P1_18  // GPIO 24
#define PIN_ECHO_3 RPI_GPIO_P1_22   // GPIO 25
#define PIN_ECHO_4 RPI_V2_GPIO_P1_32   // GPIO 12
#define PIN_ECHO_5 RPI_GPIO_P1_15  // GPIO 22
#define PIN_ECHO_6 RPI_V2_GPIO_P1_29   // GPIO 5
#define PIN_ECHO_7 RPI_GPIO_P1_11  // GPIO 17
#define PIN_ECHO_8 RPI_GPIO_P1_07  // GPIO 4


long get_time_microsec();
void pulse_trigger(int trigger_pin);
