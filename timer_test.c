#include <stdio.h>
#include <stdlib.h>
#include <bcm2835.h>
#include <sys/time.h>
#include <unistd.h>

long get_time_microsec() {
  struct timeval curr_time;
  gettimeofday(&curr_time, NULL);
  printf("%.2f\n", curr_time.tv_sec);
  return (curr_time.tv_sec * 1000000) + curr_time.tv_usec;
}

int main(void) {
	while(1) {
		printf("Timer value: %ld\n", get_time_microsec());
	}
}
