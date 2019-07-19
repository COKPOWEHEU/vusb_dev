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

int main(){
  RLED_CONFIG(); RLED_PORT &=~ RLED_MASK;
  GLED_CONFIG(); GLED_PORT &=~ GLED_MASK;
  
  uart_init(1152); //скорость 115200, отсутствие последних двух цифр - особенность библиотеки
  
  usbInit();
  usbDeviceDisconnect(); //сообщаем хосту, что на шине ничего нет
  
  uart_puts("Start\r\n");
  _delay_ms(200);
  
  usbDeviceConnect(); //сообщаем хосту, что устройство подключено
  sei();
  while(1){
  }
}

void usbInit(void){
    USB_INTR_CFG |= USB_INTR_CFG_SET;
    USB_INTR_ENABLE |= (1 << USB_INTR_ENABLE_BIT);
}
