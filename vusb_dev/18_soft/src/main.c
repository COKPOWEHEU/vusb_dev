#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "vhid.h"

const unsigned short VID = 0x16C0;
const unsigned short PID = 0x05DF;
const wchar_t MANUFACTURER[]= L"COKPOWEHEU";
const wchar_t PRODUCT[]= L"HID example";

int main(){
  static hiddevice_t *dev = NULL;
  
  dev = HidOpen(VID, PID, MANUFACTURER, PRODUCT);
  
  if(!HidIsConnected(dev)){
    printf("Can not find device\n");
    HidDisplay();
    return -1;
  }
  
  unsigned char buf[2] = {0xFF, 0};
  unsigned char res = HidWrite(dev, buf, 2);
  printf("send %.2X %.2X: %i\n", buf[0], buf[1], res);
  
  res = HidRead(dev, buf, 2);
  printf("receive %i: %.2X %.2X\n", res, buf[0], buf[1]);
  
  HidClose(dev);
}
