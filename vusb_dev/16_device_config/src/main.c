#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
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

extern unsigned usbCrc16Append(unsigned char *data, unsigned char len);
void usbInit(void);
void usbPoll(void);
static inline void usbProcessRx(uchar *data, uchar len);
static inline usbMsgLen_t usbDriverSetup(usbRequest_t *rq);
static inline void usbBuildTxBlock(void);
static inline usbMsgLen_t usbDriverDescriptor(usbRequest_t *rq);


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
  static usbMsgLen_t  usbMsgLen = USB_NO_MSG;

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
    usbProcessRx(usbRxBuf + USB_BUFSIZE + 1 - usbInputBufOffset, len);
    usbRxLen = 0;
  }
  if(usbTxLen & 0x10){
    if(usbMsgLen != USB_NO_MSG){
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
  /* usbRxToken can be:
   * 0x2d 00101101 (USBPID_SETUP for setup data)
   * 0xe1 11100001 (USBPID_OUT: data phase of setup transfer)
   * 0...0x0f for OUT on endpoint X
   */
  
  if(usbRxToken == (uchar)USBPID_SETUP){
    if(len != 8)return;
    
    usbMsgLen_t replyLen = 0;
    usbTxBuf[0] = USBPID_DATA0;
    usbTxLen = USBPID_NAK;
    uchar type = rq->bmRequestType & USBRQ_TYPE_MASK;
    uart_putc('>');
    if(type != USBRQ_TYPE_STANDARD){
      uart_putc('S');
    }else{
      uart_putc('D');
      replyLen = usbDriverSetup(rq);
    }
    if(replyLen == USB_NO_MSG){
      uart_putc('n');
    }else{
      uart_putc('m');
      if(!rq->wLength.bytes[1] && replyLen > rq->wLength.bytes[0])
        replyLen = rq->wLength.bytes[0];
      
    }
    usbMsgLen=replyLen;
  }
}

static inline usbMsgLen_t usbDriverSetup(usbRequest_t *rq){
  uchar   len  = 0, *dataPtr = usbTxBuf + 9;
  uchar   value = rq->wValue.bytes[0];
  
  dataPtr[0] = 0;
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
    len = usbDriverDescriptor(rq);
    goto skipMsgPtrAssignment;
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
  usbMsgPtr = dataPtr;
  skipMsgPtrAssignment:
  return len;
}


#define GET_DESCRIPTOR(cfgProp, staticName) \
  if(cfgProp){ \
    len = cfgProp & 0x3FFF;         \
    usbMsgPtr = (uchar *)(staticName);\
  }

#define USB_CFG_DESCR_PROPS_DEVICE  sizeof(usbDescriptorDevice)

#define  USB_CFG_VENDOR_ID       0xc0, 0x16
#define  USB_CFG_DEVICE_ID       0xdf, 0x05
#define USB_CFG_DEVICE_VERSION  0x00, 0x01

#define USB_CFG_DEVICE_CLASS        0
#define USB_CFG_DEVICE_SUBCLASS     0

#define USB_CFG_INTERFACE_CLASS     3
#define USB_CFG_INTERFACE_SUBCLASS  0
#define USB_CFG_INTERFACE_PROTOCOL  0

PROGMEM const char usbDescriptorDevice[] = {   //size=18 /* USB device descriptor */
    18,         /* sizeof(usbDescriptorDevice): length of descriptor in bytes */
    USBDESCR_DEVICE,        /* descriptor type */
    0x10, 0x01,             /* USB version supported */
    USB_CFG_DEVICE_CLASS,
    USB_CFG_DEVICE_SUBCLASS,
    0,                      /* protocol */
    8,                      /* max packet size */
    /* the following two casts affect the first byte of the constant only, but
     * that's sufficient to avoid a warning with the default values.
     */
    (char)USB_CFG_VENDOR_ID,/* 2 bytes */
    (char)USB_CFG_DEVICE_ID,/* 2 bytes */
    USB_CFG_DEVICE_VERSION, /* 2 bytes */
    1,         /* manufacturer string index */
    2,         /* product string index */
    3,         /* serial number string index */
    1,          /* number of configurations */
};

#define USB_CFG_DESCR_PROPS_CONFIGURATION   sizeof(usbDescriptorConfiguration)
PROGMEM const char usbDescriptorConfiguration[] = {    /* USB configuration descriptor */
    9,          /* sizeof(usbDescriptorConfiguration): length of descriptor in bytes */
    USBDESCR_CONFIG,    /* descriptor type */
    18 + 7 + 9, 0,// + (USB_CFG_DESCR_PROPS_HID & 0xff), 0,
                /* total length of data returned (including inlined descriptors) */
    1,          /* number of interfaces in this configuration */
    1,          /* index of this configuration */
    0,          /* configuration name string index */
    (1 << 7), //bus power
    100/2,    //максимальный ток от шины в единицах (2мА), не более 100 мА
/* interface descriptor follows inline: */
    9,          /* sizeof(usbDescrInterface): length of descriptor in bytes */
    USBDESCR_INTERFACE, /* descriptor type */
    0,          /* index of this interface */
    0,          /* alternate setting for this interface */
    1, /* endpoints excl 0: number of endpoint descriptors to follow */
    USB_CFG_INTERFACE_CLASS,
    USB_CFG_INTERFACE_SUBCLASS,
    USB_CFG_INTERFACE_PROTOCOL,
    0,          /* string index for interface */
/* HID descriptor */
    9,          /* sizeof(usbDescrHID): length of descriptor in bytes */
    USBDESCR_HID,   /* descriptor type: HID */
    0x01, 0x01, /* BCD representation of HID version */
    0x00,       /* target country code */
    0x01,       /* number of HID Report (or other HID class) Descriptor infos to follow */
    0x22,       /* descriptor type: report */
    0,/*USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH,*/ 0,  /* total length of report descriptor */
/* endpoint descriptor for endpoint 1 */
    7,          /* sizeof(usbDescrEndpoint) */
    USBDESCR_ENDPOINT,  /* descriptor type = endpoint */
    (char)0x81, /* IN endpoint number 1 */
    0x03,       /* attrib: Interrupt endpoint */
    8, 0,       /* maximum packet size */
    100, /* in ms */
};
#define USB_STRING_DESCRIPTOR_HEADER(stringLength) ((2*(stringLength)+2) | (3<<8))
#define USB_CFG_DESCR_PROPS_STRING_0    sizeof(usbDescriptorString0)
PROGMEM const char usbDescriptorString0[] = { /* language descriptor */
    4,          /* sizeof(usbDescriptorString0): length of descriptor in bytes */
    3,          /* descriptor type */
    0x09, 0x04, /* language index (0x0409 = US-English) */
};
#define USB_CFG_DESCR_PROPS_STRING_VENDOR   sizeof(usbDescriptorStringVendor)
PROGMEM const int  usbDescriptorStringVendor[] = {
    USB_STRING_DESCRIPTOR_HEADER(10),
    'C', 'O', 'K', 'P', 'O', 'W', 'E', 'H', 'E', 'U'
};
#define USB_CFG_DESCR_PROPS_STRING_PRODUCT   sizeof(usbDescriptorStringDevice)
PROGMEM const int  usbDescriptorStringDevice[] = {
    USB_STRING_DESCRIPTOR_HEADER(11),
    'H', 'I', 'D', ' ', 'e', 'x', 'a', 'm', 'p', 'l', 'e'
};
#define USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER    sizeof(usbDescriptorStringSerialNumber)
PROGMEM const int usbDescriptorStringSerialNumber[] = {
    USB_STRING_DESCRIPTOR_HEADER(3),
    '0', '.', '1'
};

static inline usbMsgLen_t usbDriverDescriptor(usbRequest_t *rq){
  usbMsgLen_t len = 0;
  
  uchar descr_type = rq->wValue.bytes[1];
  if(descr_type == USBDESCR_DEVICE){
    uart_putc('1');
    GET_DESCRIPTOR(USB_CFG_DESCR_PROPS_DEVICE, usbDescriptorDevice)
  }else if(descr_type == USBDESCR_CONFIG){
    uart_putc('2');
    GET_DESCRIPTOR(USB_CFG_DESCR_PROPS_CONFIGURATION, usbDescriptorConfiguration)
  }else if(descr_type == USBDESCR_STRING){
    uchar descr_subtype = rq->wValue.bytes[0];
    if(descr_subtype == 0){
      GET_DESCRIPTOR(USB_CFG_DESCR_PROPS_STRING_0, usbDescriptorString0)
    }else if(descr_subtype == 1){
      GET_DESCRIPTOR(USB_CFG_DESCR_PROPS_STRING_VENDOR, usbDescriptorStringVendor)
    }else if(descr_subtype == 2){
      GET_DESCRIPTOR(USB_CFG_DESCR_PROPS_STRING_PRODUCT, usbDescriptorStringDevice)
    }else if(descr_subtype == 3){
      GET_DESCRIPTOR(USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER, usbDescriptorStringSerialNumber)
    }
  }else{
    uart_putc('0');
  }
  return len;
}

static uchar usbDeviceRead(uchar *data, uchar len){
  if(len > 0){
    uchar i = len, *r = usbMsgPtr;
    do{
      uchar c = pgm_read_byte(r);
      *data++ = c;
      r++;
    }while(--i);
    usbMsgPtr = r;
  }
  return len;
}

static inline void usbBuildTxBlock(void){
  usbMsgLen_t wantLen;
  uchar       len;
  
  wantLen = usbMsgLen;
  if(wantLen > 8)wantLen = 8;
  usbMsgLen -= wantLen;
  
  usbTxBuf[0] ^= USBPID_DATA0 ^ USBPID_DATA1;
  
  len = usbDeviceRead(usbTxBuf + 1, wantLen);
  
  if(len <= 8){
    usbCrc16Append(&usbTxBuf[1], len);
    len += 4;
    if(len < 12)
      usbMsgLen = USB_NO_MSG;
  }else{
    len = USBPID_STALL;
    usbMsgLen = USB_NO_MSG;
  }
  usbTxLen = len;
}

