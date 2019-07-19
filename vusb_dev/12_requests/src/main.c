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


         uchar usbRxBuf[2*USB_BUFSIZE];
         uchar usbInputBufOffset;
         uchar usbDeviceAddr = 0;
         uchar usbNewDeviceAddr=0;
         uchar usbCurrentTok = USBPID_NAK;
volatile schar usbRxLen;
         uchar usbRxToken;
volatile uchar usbTxLen = USBPID_NAK;   /* number of bytes to transmit with next IN token or handshake token */
         uchar usbTxBuf[USB_BUFSIZE];/* data to transmit with next IN, free if usbTxLen contains handshake token */
         uchar usbConfiguration;   /* currently selected configuration. Administered by driver, but not used */
         uchar *usbMsgPtr;     /* data to transmit next -- ROM or RAM address */
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
  //проверяем не пытается ли хост послать RESET, удерживая SE0 длительное время
  for(i = 20; i > 0; i--){
    uchar usbLineStatus = USBIN & USBMASK;
    if(usbLineStatus != 0)goto isNotReset; //нет, не пытается. Это обычный EOP
  }
  // а вот если таки послали ресет - сбрасываем адрес устройства. Наверное, надо бы
  // и внутреннее состояние тоже сборосить, но вроде и не обязательно. Надо будет - сам
  // переконфигурирует!
  usbNewDeviceAddr = 0;
  usbDeviceAddr = 0;
  isNotReset:{}
}


/* usbProcessRx() is called for every message received by the interrupt
 * routine. It distinguishes between SETUP and DATA packets and processes
 * them accordingly.
 */
static inline void usbProcessRx(uchar *data, uchar len){
  usbRequest_t    *rq = (void *)data;
  /* usbRxToken can be:
   * 0x2d 00101101 (USBPID_SETUP for setup data)
   * 0xe1 11100001 (USBPID_OUT: data phase of setup transfer)
   * 0...0x0f for OUT on endpoint X
   */
  
  if(usbRxToken == (uchar)USBPID_SETUP){
    if(len != 8)return;    // Setup size must be always 8 bytes. Ignore otherwise.
    
    usbTxBuf[0] = USBPID_DATA0;         // initialize data toggling
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
  }
}


/* usbDriverSetup() is similar to usbFunctionSetup(), but it's used for
 * standard requests instead of class and custom requests.
 */
static inline usbMsgLen_t usbDriverSetup(usbRequest_t *rq){
  uchar   len  = 0, *dataPtr = usbTxBuf + 9;  // there are 2 bytes free space at the end of the buffer
  uchar   value = rq->wValue.bytes[0];
  
  dataPtr[0] = 0; // default reply common to USBRQ_GET_STATUS and USBRQ_GET_INTERFACE
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
    goto skipMsgPtrAssignment;
  }else if(req == USBRQ_GET_CONFIGURATION){    // 8
    uart_putc('c');
    dataPtr = &usbConfiguration;
    len = 1;
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
  usbMsgPtr = dataPtr;
  skipMsgPtrAssignment:
  return len;
}
