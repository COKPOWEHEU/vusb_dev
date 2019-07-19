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

typedef union usbWord{
    unsigned    word;
    uchar       bytes[2];
}usbWord_t;
typedef struct usbRequest{
    uchar       bmRequestType;
    uchar       bRequest;
    usbWord_t   wValue;
    usbWord_t   wIndex;
    usbWord_t   wLength;
}usbRequest_t;

typedef struct usbTxStatus{
    volatile uint8_t   len;
    uint8_t   buffer[USB_BUFSIZE];
}usbTxStatus_t;

usbTxStatus_t   usbTxStatus1;

void usbInit(void);
void usbPoll(void);
static inline void usbProcessRx(uchar *data, uchar len);
static inline usbMsgLen_t usbDriverSetup(usbRequest_t *rq);
static inline void usbBuildTxBlock(void);

         uchar usbRxBuf[2*USB_BUFSIZE];
         uchar usbInputBufOffset;
         uchar usbDeviceAddr = 0;
         uchar usbNewDeviceAddr=0;
         uchar usbCurrentTok = USBPID_NAK;
volatile schar usbRxLen;
         uchar usbRxToken;
volatile uchar usbTxLen = USBPID_NAK;
         uchar usbTxBuf[USB_BUFSIZE];
         uchar usbConfiguration;
         uchar *usbMsgPtr;
  static usbMsgLen_t  usbMsgLen = USB_NO_MSG; /* remaining number of bytes */
  static uchar usbMsgFlags;    /* flag values see below */

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

  while(1){
    usbPoll();
  }
}

void usbInit(void){
  USB_INTR_CFG |= USB_INTR_CFG_SET;
  USB_INTR_ENABLE |= (1 << USB_INTR_ENABLE_BIT);
}


void usbPoll(void){
  schar   len;
  uchar   i;
  
  len = usbRxLen - 3;
  if(len >= 0){
    //передаем в функцию буфер с принятыми данными (после приема прерывание переключило буферы)
    usbProcessRx(usbRxBuf + USB_BUFSIZE + 1 - usbInputBufOffset, len);
    usbRxLen = 0;       // mark rx buffer as available
  }
  if(usbTxLen & 0x10){    // transmit system idle
    if(usbMsgLen != USB_NO_MSG){    // transmit data pending?
      usbBuildTxBlock();
    }
  }

  for(i = 20; i > 0; i--){
    uchar usbLineStatus = USBIN & USBMASK;
    if(usbLineStatus != 0)goto isNotReset;
  }
  usbNewDeviceAddr = 0;
  usbDeviceAddr = 0;
  isNotReset:{}
}

static inline void usbProcessRx(uchar *data, uchar len){
  usbRequest_t    *rq = (void *)data;
  
  if(usbRxToken == (uchar)USBPID_SETUP){
    if(len != 8)return;
    
    usbTxBuf[0] = USBPID_DATA1;         // initialize data toggling
    usbTxLen = USBPID_NAK;              // abort pending transmit
    usbMsgFlags = 0;
    uchar type = rq->bmRequestType & USBRQ_TYPE_MASK;
    uart_putc('>');
    if(type != USBRQ_TYPE_STANDARD){    // standard requests are handled by driver
      uart_putc('S');
    }else{
      uart_putc('D');
      usbDriverSetup(rq);
    }
    usbMsgLen=0;
  }
}

static inline usbMsgLen_t usbDriverSetup(usbRequest_t *rq){
  uchar   value = rq->wValue.bytes[0];
  
  uchar req = rq->bRequest;
  if(req == USBRQ_GET_STATUS){                 // 0
    uart_putc('s');
  }else if(req == USBRQ_SET_ADDRESS){          // 5
    usbNewDeviceAddr = value;
    char str[10];
    strhex(value, str);
    str[2]='\n';
    str[3]=0;
    uart_puts(str);
  }else if(req == USBRQ_GET_DESCRIPTOR){       // 6
    uart_putc('d');
  }else if(req == USBRQ_GET_CONFIGURATION){    // 8
    uart_putc('c');
  }else if(req == USBRQ_SET_CONFIGURATION){    // 9
    uart_putc('C');
    usbConfiguration = value;
  }else if(req == USBRQ_GET_INTERFACE){        // 10
    uart_putc('i');
  }else if(req == USBRQ_SET_INTERFACE){        // 11
    uart_putc('I');
  }else{
    uart_putc('?');
  }
  return 0;
}

//вызывается когда у нас есть данные на передачу, причем буфер передачи пуст
static inline void usbBuildTxBlock(void){
  usbMsgLen = USB_NO_MSG;
  usbTxLen = 4;
}
