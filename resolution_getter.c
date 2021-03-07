// Code based off https://raspberrycompote.blogspot.com/2012/12/low-level-graphics-on-raspberry-pi-part_9509.html

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "resolution_getter.h"

struct resolution get_resolution() {
  struct resolution res;
  int fbfd = 0; // framebuffer filedescriptor
  struct fb_var_screeninfo var_info;

  // Open the framebuffer device file for reading and writing
  fbfd = open("/dev/fb0", O_RDWR);
  if (fbfd == -1) {
    printf("Error: cannot open framebuffer device.\n");
    res.xres = -1;
    res.yres = -1;
    return res;
  }

  // Get variable screen information
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &var_info)) {
    printf("Error reading variable screen info.\n");
  }

  res.xres = var_info.xres;
  res.yres = var_info.yres;
  
  // close file  
  close(fbfd);
  
  return res;

}
