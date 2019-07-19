#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usbconfig.h"
#include "uart.h"

void strhex(uint8_t x, char *str){
  uint8_t lh = x & 0x0F;
  x >>= 4;
  if(x > 9)str[0]=(x-0x0A+'A'); else str[0]=(x+'0');
  if(lh> 9)str[1]=(lh-0x0A+'A');else str[1]=(lh+'0');
}

void usbInit(void);


volatile uchar usbRxBuf[2*USB_BUFSIZE];
volatile uchar usbDeviceAddr = 0;
volatile uchar usbCurrentTok;

int main(){
  RLED_CONFIG(); RLED_PORT &=~ RLED_MASK;
  GLED_CONFIG(); GLED_PORT &=~ GLED_MASK;
  
  uart_init(1152);
  
  usbInit();
  usbDeviceDisconnect();
  
  uart_puts("Start\r\n");
  _delay_ms(200);
  
  usbDeviceConnect();
  sei();
  
  char str[30];
  while(1){
    if(usbRxBuf[0]!=0){
      strhex(usbRxBuf[0], str); str[2]='|'; usbRxBuf[0]=0;
      strhex(usbRxBuf[1], str+3); str[5]='|'; usbRxBuf[1] = 0xAA;
      strhex(usbRxBuf[2], str+6); str[8]='|'; usbRxBuf[2] = 0xAA;
      strhex(usbRxBuf[3], str+9); str[11]='|'; usbRxBuf[3] = 0xAA;
      strhex(usbRxBuf[4], str+12);str[14]='|'; usbRxBuf[4] = 0xAA;
      strhex(usbRxBuf[5], str+15);str[17]='|'; usbRxBuf[5] = 0xAA;
      strhex(usbRxBuf[6], str+18);str[20]='|'; usbRxBuf[6] = 0xAA;
      strhex(usbRxBuf[7], str+21);str[23]='|'; usbRxBuf[7] = 0xAA;
      strhex(usbRxBuf[8], str+24);usbRxBuf[8] = 0;
      str[26]='\r';
      str[27]='\n';
      str[28]=0;
      
      uart_puts(str);
    }
  }
}

void usbInit(void){
    USB_INTR_CFG |= USB_INTR_CFG_SET;
    USB_INTR_ENABLE |= (1 << USB_INTR_ENABLE_BIT);
}
