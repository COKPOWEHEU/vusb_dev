#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "usbdrv.h"

//это отладочно-дамонстрационные обозначения, в коде vusb они не используются
#define RLED_PORT PORTD
#define RLED_BIT 6
#define RLED_CONFIG() DDRD |= (1<<(RLED_BIT))
#define RLED_MASK (1<<(RLED_BIT))

#define GLED_PORT PORTD
#define GLED_BIT 7
#define GLED_CONFIG() DDRD |= (1<<(GLED_BIT))
#define GLED_MASK (1<<(GLED_BIT))

#define BTN_CONFIG() do{ DDRD &=~(1<<5); PORTD |= (1<<5); }while(0)
#define BTN_STATE ((PIND & (1<<5))!=0)

PROGMEM const char usbHidReportDescriptor[22] = { // USB report descriptor         // дескриптор описывает структуру пакета данных для обмена
    0x06, 0x00, 0xff,                       // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,                             // USAGE (Vendor Usage 1)
    0xa1, 0x01,                             // COLLECTION (Application)
    0x15, 0x00,                             //    LOGICAL_MINIMUM (0)        // min. значение для данных
    0x26, 0xff, 0x00,                       //    LOGICAL_MAXIMUM (255)      // max. значение для данных, 255 тут не случайно, а чтобы уложитьс€ в 1 байт
    0x75, 0x08,                             //    REPORT_SIZE (8)            // информация передается порциями, это размер одного "репорта" 8 бит
    0x95, 2,    //    REPORT_COUNT
    0x09, 0x00,                             //    USAGE (Undefined)
    0xb2, 0x02, 0x01,                       //    FEATURE (Data,Var,Abs,Buf)
    0xc0                                    // END_COLLECTION
};

uchar pdata[2];
static uchar    currentAddress;
static uchar    bytesRemaining;


/* usbFunctionRead() вызывается когда хост запрашивает порцию данных от устройства
 * для дополнительной информации см. документацию в usbdrv.h
 */
uchar   usbFunctionRead(uchar *data, uchar len){
  if(len > bytesRemaining)len = bytesRemaining;
  
  uchar *buffer = (uchar*)&pdata;
  
  if(!currentAddress){        // pdata<=???
    pdata[0] = 0x02;
    if(BTN_STATE){
      pdata[1] = 1;
    }else{
      pdata[1] = 0;
    }
  }
  
  uchar j;
  for(j=0; j<len; j++)data[j] = buffer[j+currentAddress];
  
  currentAddress += len;
  bytesRemaining -= len;
  return len;
}

/* usbFunctionWrite() вызываетс€ когда хост отправл€ет порцию данных к устройству
 * для дополнительной информации см. документацию в usbdrv.h
 */
uchar   usbFunctionWrite(uchar *data, uchar len){
  if(bytesRemaining == 0)return 1;               /* конец передачи */
    
  if(len > bytesRemaining)len = bytesRemaining;
    
  uchar *buffer = (uchar*)&pdata;
  
  uchar j;
  for(j=0; j<len; j++)buffer[j+currentAddress] = data[j];
  
  currentAddress += len;
  bytesRemaining -= len;
  
  if(bytesRemaining == 0){     // ???<=pdata
    RLED_PORT ^= RLED_MASK;
    if((pdata[0] & 0xFE) == 0xFE){
      if(pdata[0] & 0x01)RLED_PORT ^= GLED_MASK;
    }
  }
  return bytesRemaining == 0; /* 0 означает, что есть еще данные */
}

usbMsgLen_t usbFunctionSetup(uchar data[8]){
  usbRequest_t    *rq = (void *)data;
  
  if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID устройство */
    if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
      // у нас только одна разновидность репорта, можем игнорировать report-ID
      bytesRemaining = 2;
      currentAddress = 0;
      return USB_NO_MSG;  // используем usbFunctionRead() для отправки данных хосту
    }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
      // у нас только одна разновидность репорта, можем игнорировать report-ID
      bytesRemaining = 2;
      currentAddress = 0;
      return USB_NO_MSG;  // используем usbFunctionWrite() для получения данных от хоста
    }
  }else{
    /* остальные запросы мы просто игнорируем */
  }
  return 0;
}

int main(void){
  RLED_CONFIG(); RLED_PORT &=~ RLED_MASK;
  GLED_CONFIG(); GLED_PORT &=~ GLED_MASK;
  BTN_CONFIG();
  
  usbInit();
  usbDeviceDisconnect();
  
  _delay_ms(255);
  
  usbDeviceConnect();
  sei();
  
  while(1){
    usbPoll(); // эту функцию надо регулярно вызывать с главного цикла, максимальная задержка между вызовами - 50 ms
  }
}
