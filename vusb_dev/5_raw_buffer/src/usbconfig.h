#ifndef __usbconfig_h_included__
#define __usbconfig_h_included__

#define USB_CFG_IOPORTNAME      D
#define USB_CFG_DMINUS_BIT      3
#define USB_CFG_DPLUS_BIT       2

#define USB_CFG_PULLUP_IOPORTNAME   D
#define USB_CFG_PULLUP_BIT          4

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

#define USB_BUFSIZE     11

#define uchar unsigned char
#define schar signed char

#ifdef USB_CFG_PULLUP_IOPORTNAME
  #define usbDeviceConnect()      ((USB_PULLUP_DDR |= (1<<USB_CFG_PULLUP_BIT)), \
                                  (USB_PULLUP_OUT |= (1<<USB_CFG_PULLUP_BIT)),\
                                  (USBDDR &= ~(1<<USB_CFG_DMINUS_BIT)))
  #define usbDeviceDisconnect()   ((USB_PULLUP_DDR &= ~(1<<USB_CFG_PULLUP_BIT)), \
                                  (USB_PULLUP_OUT &= ~(1<<USB_CFG_PULLUP_BIT)),\
                                  (USBDDR |= (1<<USB_CFG_DMINUS_BIT)))
#else
  #define usbDeviceConnect()      (USBDDR &= ~(1<<USB_CFG_DMINUS_BIT))
  #define usbDeviceDisconnect()   (USBDDR |= (1<<USB_CFG_DMINUS_BIT))
#endif


#define USB_INTR_CFG MCUCR
#define USB_INTR_ENABLE  GIMSK
#define USB_INTR_CFG_SET (1 << ISC01) /*Int0 on falling edge*/
#define USB_INTR_ENABLE_BIT  INT0


#define USBOUT          PORTD
#define USB_PULLUP_OUT  PORTD
#define USBIN           PIND
#define USBDDR          DDRD
#define USB_PULLUP_DDR  DDRD

#define USBMINUS    USB_CFG_DMINUS_BIT
#define USBPLUS     USB_CFG_DPLUS_BIT
#define USBIDLE     (1<<USB_CFG_DMINUS_BIT) /* value representing J state */
#define USBMASK     ((1<<USB_CFG_DPLUS_BIT) | (1<<USB_CFG_DMINUS_BIT))  /* mask for USB I/O bits */

#endif
