/* ���: usbconfig.h
 * ������: V-USB, ������� AVR USB	
 * �����: Christian Starkjohann
 * �������: microsin.ru + XANDER
 * ���� ��������: 2005-04-01
 * ���������: 4
 * Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
 * ��������: GNU GPL v2 (��. License.txt) ��� ������������� (CommercialLicense.txt)
 * �������: $Id: usbconfig-prototype.h 785 2010-05-30 17:57:07Z cs $
 */
 
#ifndef __usbconfig_h_included__
#define __usbconfig_h_included__

/* �������� ��������:
 * ���� ���� �������� �������� ������������ (�� ���������� �������������) ���
 * �������� USB. �� ������������ V-USB ��� ������� USB D+, ��������������� �
 * ����� D ��� 2 (������� ����� �������� ������ ����������� ���������� 0 [INT0]
 * �� ������ �����) � ������� USB D- � ����� D ��� 4. �� ������ ������������
 * ��� ������� �� ����� ������ ����, ������ D+ ������ ���� ����� ��������� ��
 * ���� ����������� ���������� INT0, ������� ����� ��������� ���������,
 * ��. ������ � ����� �����).
 */

/* ------------------------- ���������� ��������� -------------------------- */

#define USB_CFG_IOPORTNAME      D
/* ������ ����, � �������� ���������� ���� USB. ���� �� ��������������� "B",
 * ����� �������������� �������� PORTB, PINB � DDRB.
 */
#define USB_CFG_DMINUS_BIT      3
/* ��� ����� ���� � USB_CFG_IOPORT, ���� ��������� ������ D-, ����� 
 * �������������� ����� ��� � �����.
 */
#define USB_CFG_DPLUS_BIT       2
/* ��� ����� ���� � USB_CFG_IOPORTNAME, ���� ��������� ������ D+, �����
 * �������������� ����� ��� � �����. ����������, ������� �� ��������, ��� D+
 * ������ ���� ����� ����������� � ����� ���������� INT0! [�� ������ �����
 * ������������ ������ ����������, ��. ������ "Optional MCU Description" �����,
 * ��� �� ������ ������������ D- � ����������, ��� ��� ���������� ���� ��
 * ����������� ����� USB_COUNT_SOF. ���� �� ����������� D- ��� ����������,
 * ��� ����� ����������� ����� �� �������� Start-Of-Frame ������
 * �����������.]
 */
#define USB_CFG_CLOCK_KHZ       (F_CPU/1000)
/* �������� ������� ���� AVR � ����������. ���������� �������� 12000, 15000,
 * 16000, 16500 � 20000. ������ ���� �� 16.5 ��� �� ������� ������, ���
 * ��������� ���� ������� +/- 1% �� �����������. ��� ������ ������� �������
 * �������� 2000 ppm, � � ���� ������ ����������� ����� �����!
 * ��������� F_CPU ������ ���� ���������� � �������, �� ����� ����� ������
 * �������.
 */ 
#define USB_CFG_CHECK_CRC       0
/* ���������� 1 ���� ������ ����� ������� V-USB �������� ����������� ��������
 * ������� ������ (CRC ����). �������� CRC ������� ���������� ����� ����� ���
 * ���� � �� ������ ������ �������������� ������ ��� ������� 18 MHz. ��� ���
 * ����������� ����������� ������� 18 MHz ��� ��������� ���� �����.
 */

/* ----------------- �������������� ���������� ��������� ------------------- */

#define USB_CFG_PULLUP_IOPORTNAME   D
/* ���� �� ������������ 1.5k pullup �������� �� D- � ����� ����� ������ V+, 
 * �� ������ ������������ � ����������� ���������� ���������� ����� �������
 * ������ usbDeviceConnect() � usbDeviceDisconnect() (��. usbdrv.h).
 * ��� ��������� ������ ����, � �������� ��������� pullup ��������.
 */
#define USB_CFG_PULLUP_BIT          4
/* This constant defines the bit number in USB_CFG_PULLUP_IOPORT (defined
 * above) where the 1.5k pullup resistor is connected. See description
 * above for details.
 * ��� ��������� ���������� ����� ���� ����� � USB_CFG_PULLUP_IOPORTNAME (�����
 * ����) ���� ��������� 1.5k pullup ��������. ��. ��������, ��������� �����.
 */

/* ----------------------- �������������� �������� ------------------------- */

#define USB_CFG_HAVE_INTRIN_ENDPOINT    1
/* ������� ����� 1, ���� �� ������ �������������� ������ � ����� ���������
 * ������� (endpoints): default control endpoint 0 � interrupt-in endpoint
 * (����� ������ endpoint �����).
 */
#define USB_CFG_HAVE_INTRIN_ENDPOINT3   0
/* ������� ����� 1, ���� �� ������ �������������� ������ � ����� ���������
 * ������� (endpoints): default control endpoint 0, interrupt-in endpoint 3
 * (��� �����, ������������������ �����) � catch-all default interrupt-in
 * endpoint ��� ������� �����.
 * �� ������ ����� ������ USB_CFG_HAVE_INTRIN_ENDPOINT � 1 ��� ���� �����.
 */
#define USB_CFG_EP3_NUMBER              3
/* ���� ��� ���������� �������� ����� (endpoint) 3 ������������, ����� ������
 * �� ���������������� �� ����� ������ ����� �������� ����� (�� ����������� 0).
 * ���� �� ������, �� �� ��������� 3.
 */
/* #define USB_INITIAL_DATATOKEN           USBPID_DATA1 */
/* ���� define ���������� ������� ������ (startup condition) ��� ������������
 * ������ (data toggling) � ������ �������� ����� interrupt/bulk 1 � 3.
 * �� ��������� USBPID_DATA1. ��������� ����� ������������� ����� ���������
 * ����� ������, ������ ����� ������������  c ��������������� ���������
 * �� ���� ������������!
 */
#define USB_CFG_IMPLEMENT_HALT          0
/* ������� ����� 1, ���� �� ������ ����� ����������� ����������� ENDPOINT_HALT
 * ��� endpoint 1 (interrupt endpoint). �������� �� ��, ��� ��� ��� �����������
 * ����� �� ������������, ��� ��������� �� ���������. �� ����� ��� ����� �
 * ������������ ������ ��� ��� ������� ����������� ��������� ���.
 */
#define USB_CFG_SUPPRESS_INTR_CODE      0
/* ������� ����� 1, ���� �� ������ ���������� interrupt-in endpoints, �� ��
 * ������ ���������� ����� ������ � �� �������. ���� ��� ����� ��������, ��
 * ������� usbSetInterrupt() � usbSetInterrupt3() ����� ���������. �������
 * ��� ������������� interrupt-in endpoints ��� ����������� ����������
 * (�������� HID), �� ��� �������� ����� � �� �������. �������� ��������
 * ���� ������ ��� ���� � ����������� ������ ������� �������� ������.
 */
#define USB_CFG_INTR_POLL_INTERVAL      100
/* ���� �� �������������� ������ � endpoint 1 (interrupt-in), �� ����� ��������
 * �������� ������ (poll interval). �������� ������� � ������������ � �� ������
 * ���� ������ 10 ms ��� ��������������� ���������.
 */
#define USB_CFG_IS_SELF_POWERED         0
/* ������� ����� 1 ���� ���������� ����� ����������� �������� �������.
 * ��������� � 0 ������������� ����������, ����������� �� ���� USB.
 */
#define USB_CFG_MAX_BUS_POWER           100
/* ���������� ��� ���������� �� max �������� ����������� �������� ������
 * ���������� �� ���� USB. �������� ������� � �����������.
 */
#define USB_CFG_IMPLEMENT_FN_WRITE      1
/* ������� ����� 1, ���� �� ������, ����� ������� usbFunctionWrite()
 * ���������� ��� ������� control-out. ������� 0 ���� ��� ��� �� �����,
 * ��� �������� ���������� ��������� ����.
 */
#define USB_CFG_IMPLEMENT_FN_READ       1
/* ������� 1 ���� ��� ����� ���������� ����������� ������� ������, �������
 * ������������ "�� ����" ��� ������ usbFunctionRead(). ���� ��� ����� ������
 * ��������� ������ �� ������������ ������, ������� 0 � ����������� ������ ��
 * usbFunctionSetup(). ��� ��������� ��������� ���������� ����.
 */
#define USB_CFG_IMPLEMENT_FN_WRITEOUT   0
/* ������� 1 ���� �� ������ ������������ interrupt-out (��� bulk out) 
 * endpoints. �� ������ ����������� ������� usbFunctionWriteOut(), �������
 * ��������� ��� interrupt/bulk ������, ������������ � ����� endpoint,
 * �������� �� 0. ����� endpoint ����� ����� � 'usbRxToken'.
 */
#define USB_CFG_HAVE_FLOWCONTROL        0
/* ������� 1, ���� �� ������ ����������� ���������� ������� (flowcontrol)
 * ����� ������ USB. ������� �������� ������� usbDisableAllRequests() �
 * usbEnableAllRequests() � usbdrv.h.
 */
#define USB_CFG_DRIVER_FLASH_PAGE       0
/* ���� ���������� ����� ������ ��� 64 kBytes ���� ������, ���������� ���
 * � 64 k �������� ��� ��������� ��������� (�����������) ��������. �������
 * �������: ���������� � 1 ��� ����������� (boot loaders) �� ATMega128.
 */
#define USB_CFG_LONG_TRANSFERS          0
/* ������� 1, ���� �� ������ ����������/��������� ����� �������� ����� 254 ����
 * � ��������� control-in ��� control-out ��������. �������� ��������, ��� ���
 * ����������� ����������� ������ ��������.
 */
/* #define USB_RX_USER_HOOK(data, len)     if(usbRxToken == (uchar)USBPID_SETUP) blinkLED(); */
/* ���� macro �������� ������ (hook), ���� �� ������ ������� �����
 * ��������������. ���� ������ �����, �� ����������� � ������ ���������
 * ������������ ���������. ���� �� "�����" (����������) ��������� � �� ������
 * �������� ���������� ��������� �� ���������, �������� ������� ������� �����
 * ����� ��������. ���� �� ��������� ���������� (����� �������) - �������
 * ��������� ����������� ��� ������ ������.
 */
/* #define USB_RESET_HOOK(resetStarts)     if(!resetStarts){hadUsbReset();} */
/* ���� macro �������� ����� ��� ��������� ������� USB RESET. �� ����� ����
 * ��������, ����������� �������� ����� ��������� RESET �� ���������
 * ��������� RESET.
 */
/* #define USB_SET_ADDRESS_HOOK()              hadAddressAssigned(); */
/* ���� macro (���� �����) ����������� ����� ������ ������ USB SET_ADDRESS.
 */
#define USB_COUNT_SOF                   0
/* ������� ����� 1, ���� ��� ����� ���������� ���������� "usbSofCount",
 * � ������� �������������� ������ SOF. ��� ����������� ������� �����������
 * ����������� ���������� � D- ������ D+.
 */
/* #ifdef __ASSEMBLER__
 * macro myAssemblerMacro
 *     in      YL, TCNT0
 *     sts     timer0Snapshot, YL
 *     endm
 * #endif
 * #define USB_SOF_HOOK                    myAssemblerMacro
 * This macro (if defined) is executed in the assembler module when a
 * Start Of Frame condition is detected. It is recommended to define it to
 * the name of an assembler macro which is defined here as well so that more
 * than one assembler instruction can be used. The macro may use the register
 * YL and modify SREG. If it lasts longer than a couple of cycles, USB messages
 * immediately after an SOF pulse may be lost and must be retried by the host.
 * What can you do with this hook? Since the SOF signal occurs exactly every
 * 1 ms (unless the host is in sleep mode), you can use it to tune OSCCAL in
 * designs running on the internal RC oscillator.
 * Please note that Start Of Frame detection works only if D- is wired to the
 * interrupt, not D+. THIS IS DIFFERENT THAN MOST EXAMPLES!
 */
#define USB_CFG_CHECK_DATA_TOGGLING     0
/* ���������� � 1 ���� ������ ����������� ������������� (duplicate) ������
 * ������ ������������ ������. ���������� ��������� ���� � ���������� ������
 * �����, ����� ���� �� ������� ACK. �������, ����������� �������� ����� �
 * usbFunctionWriteOut() � usbFunctionWrite(). ����������� ���������� 
 * usbCurrentDataToken � ����������� ���������� ��� ������� 
 * control- � out-endpoint ��� �������� �� ������������� ������.
 */
#define USB_CFG_HAVE_MEASURE_FRAME_LENGTH   0
/* ������� ����� 1, ���� ��� ����� ������� usbMeasureFrameLength() - ����� ���
 * ��������������. ��� ������� ����� �������������� ��� ���������� ����������
 * RC ���������������� AVR.
 */
#define USB_USE_FAST_CRC                0
/* ������������ ����� ����� ��� ���������� ��� ��������� CRC. ���� �������,
 * ������ ������� �� ������. ����������� ������� ���� � ��������� ���������
 * �� ��������� � ���������. ������� ���������� ������� 31 ���� �� ����, �
 * ������� ������� 61 - 69 ������. ������� ���������� ����� ���� �� 32 �����
 * ���� ������� ��� ��������� ������ � ������� AVR �� ��������� ������� �
 * ������� ������������.
 */

/* ---------------- ���������, ����������� USB-���������� ------------------ */

#define  USB_CFG_VENDOR_ID       0xc0, 0x16 /* = 0x16c0 = 5824 = voti.nl */
/* USB ������ ID ��� ����������, ������� ���� ���� ������. ���� ��
 * ���������������� ���� ����������� Vendor ID, ������� ��� �����. �
 * ��������� ������ ����������� ���� �� �������� ��������������� ���
 * VID/PID obdev. ���������� ������� USB-IDs-for-free.txt!
 */
#define  USB_CFG_DEVICE_ID       0xdf, 0x05 /* obdev's shared PID for HIDs */
/* ��� ID ��������, ������� ���� ���� ������. �� ���������������� � ���������
 * vendor ID. ���� �� ���������������� ���� ����������� VID �� usb.org, ���
 * ���� ����������� ��������� �� PID �� ����-������ ���, ������� ��� �����.
 * ����� ����������� ��������������� obdev �������� ���������������� 
 * ���� VID/PID. ���������� ������� USB-IDs-for-free.txt!
 */
#define USB_CFG_DEVICE_VERSION  0x00, 0x01
/* ����� ������ ����������: ������� ������� �����, ����� ������� �����.
 */
#define USB_CFG_VENDOR_NAME     'C', 'O', 'K', 'P', 'O', 'W', 'E', 'H', 'E', 'U'
#define USB_CFG_VENDOR_NAME_LEN 10
/* ����� ��������� ��� ������� (vendor name), ������������ �����������.
 * ��� ������ ��������������� ��� ������ �������� � ���������
 * ��������, � USB_CFG_VENDOR_NAME_LEN ������ �� ����������. �������
 * ���������������� ��� Unicode (UTF-16). ���� ��� �� ����� ��� �������,
 * ��������������� ���� ������. ������ ���������� ��� �������, ���������� ����
 * �������� ��� Internet, ���� �� ����������� �������� ���������������� ����
 * obdev VID/PID. �� �������� ����������� � ����� USB-IDs-for-free.txt.
 */
#define USB_CFG_DEVICE_NAME    'H', 'I', 'D', ' ', 'e', 'x', 'a', 'm', 'p', 'l', 'e'
#define USB_CFG_DEVICE_NAME_LEN 11
/* ����� ����������� ��� ���������� (device name) ����� �� ��������, ��� � �
 * ���������� ��������� ����������� ��� �������. ���� ��� �� ����� ���
 * ����������, ��������������� ���� ������. ��. ���� USB-IDs-for-free.txt
 * ����� ����������� �����, ���� �� ����������� �������� ����������������
 * ���� obdev VID/PID.
 */
#define USB_CFG_SERIAL_NUMBER   '0', '.', '1'
#define USB_CFG_SERIAL_NUMBER_LEN   3
/* �������� ����� (serial number). ���� �������� ����� ��� �� �����,
 * ��������������� ������. ����� ��������� �������� ������������ serial number
 * �� ������ ������������, ������������ �� ������� ����������. ��. ������,
 * ����������� �������� ����������� �����, ��� ������� ��� ������� ������
 * ���������� ������������� USB (USB descriptors), ������ ��� string
 * descriptor ��� ��������� ������.
 */
#define USB_CFG_DEVICE_CLASS        0
#define USB_CFG_DEVICE_SUBCLASS     0
/* ��. ������������ USB, ���� �� ������ ������������ ���������� �
 * ������������� ������ ��������� (device class). ����� 0xff ��������
 * "������������� ��� �������" ("vendor specific").
 */
#define USB_CFG_INTERFACE_CLASS     3
#define USB_CFG_INTERFACE_SUBCLASS  0
#define USB_CFG_INTERFACE_PROTOCOL  0
/* ��. ������������ USB, ���� �� ������, ����� ���������� �������� �����������
 * ������������� ������ ��� ���������. ��������� ������ ������ ���� �����������
 * �� ������ ����������:
 * HID class 3, ����������� ���������� � ���������� (����� ��������� ��������!)
 * CDC class 2, ����������� �������� 2 � �������� 1 ��� ACM
 */
#define USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH    22
/* ������� ����� ����� HID report descriptor, ���� �� ����������� HID
 * ����������. ����� �� ��������� ���� �������� ��� ���������� ��� � 0.
 * ���� ����������� ���� ��������, �� �� ������ �������� ���������� ������
 * PROGMEM, ��������� "usbHidReportDescriptor", � ���, ������� ��������
 * report descriptor. �� �������� ������� ������ � ���� �������� �
 * ������������!
 */

/* #define USB_PUBLIC static */
/* ����������� ���� define, ���� �� ����������� ��������� #include usbdrv.c
 * ������ ����������. ��� ������� �������� ��������� ���� flash-������.
 */

/* --------------- ������ ���������� ����� ����������� USB ----------------- */
/* ���� �� �� ������ ������������ ����������� �� ��������� �������� USB,
 * �� ������ ������������ ���� �����������. ��� ����� ���� ������� 
 * ��� (1) ����������� ������ ������������� ����� � ������ flash, 
 * (2) ����������� ������ ������������� ����� � RAM, ��� (3) ������������ ��
 * ����� ���������� � ������� usbFunctionDescriptor(). ������� usbdrv.h ���
 * ������� ���������� �� ����� ������������. ��������� ������������
 * ��������������� ����� �������� �����������. ���� �� ������ �������� ���
 * ���� ��� ����� 0, ������������ ���������� �� ���������.
 * �������� ��������:
 *   + USB_PROP_IS_DYNAMIC: ������ ��� ����������� ������ ���� ��������� (fetched)
 *     �� ����� ���������� ����� usbFunctionDescriptor(). ���� ������������
 *     �������� usbMsgPtr, ������ �� ��������� � FLASH.
 *     �������� �������� USB_PROP_IS_RAM ���� ������ RAM ���������.
 *   + USB_PROP_IS_RAM: ������, ������������ usbFunctionDescriptor() ���
 *     ��������� � ����������� ������ RAM, �� � ������ flash.
 *   + USB_PROP_LENGTH(len): ���� ������ � ����������� ������ (RAM ��� flash),
 *     ������� ������ ����� ����� �����������. ���������� ��� �� ���� ������ �� ������,
 *     ���������� ��� ������������� (��. �����).
 * ������ ����������� ���� ������������ (������ ���� �������������� PROGMEM � flash):
 *   char usbDescriptorDevice[];
 *   char usbDescriptorConfiguration[];
 *   char usbDescriptorHidReport[];
 *   char usbDescriptorString0[];
 *   int usbDescriptorStringVendor[];
 *   int usbDescriptorStringDevice[];
 *   int usbDescriptorStringSerialNumber[];
 * ������ ����������� �� ����� ���� ������������� ����������, ��� ������
 * ��������������� ����������� �� ����� ����������.  
 *
 * �������� ������������ ������������ �� ��� ���� ������������ ������, ��������:
 * #define USB_CFG_DESCR_PROPS_DEVICE   (USB_PROP_IS_RAM | USB_PROP_LENGTH(18))
 *
 * ��������� ����������� ������:
 *   USB_CFG_DESCR_PROPS_DEVICE
 *   USB_CFG_DESCR_PROPS_CONFIGURATION
 *   USB_CFG_DESCR_PROPS_STRINGS
 *   USB_CFG_DESCR_PROPS_STRING_0
 *   USB_CFG_DESCR_PROPS_STRING_VENDOR
 *   USB_CFG_DESCR_PROPS_STRING_PRODUCT
 *   USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER
 *   USB_CFG_DESCR_PROPS_HID
 *   USB_CFG_DESCR_PROPS_HID_REPORT
 *   USB_CFG_DESCR_PROPS_UNKNOWN (for all descriptors not handled by the driver)
 *
 * ������� ��� string ����������� �� ������ ������, ��� ��������
 * Unicode strings prefixed with a 2 byte header. ������:
 * int  serialNumberDescriptor[] = {
 *     USB_STRING_DESCRIPTOR_HEADER(6),
 *     'S', 'e', 'r', 'i', 'a', 'l'
 * };
 */

#define USB_CFG_DESCR_PROPS_DEVICE                  0
#define USB_CFG_DESCR_PROPS_CONFIGURATION           0
#define USB_CFG_DESCR_PROPS_STRINGS                 0
#define USB_CFG_DESCR_PROPS_STRING_0                0
#define USB_CFG_DESCR_PROPS_STRING_VENDOR           0
#define USB_CFG_DESCR_PROPS_STRING_PRODUCT          0
#define USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER    0
#define USB_CFG_DESCR_PROPS_HID                     0
#define USB_CFG_DESCR_PROPS_HID_REPORT              0
#define USB_CFG_DESCR_PROPS_UNKNOWN                 0

/* --------------------- �������������� ��������� MCU ---------------------- */
/* ��������� ������������ �������� �� ��������� � usbdrv.h. ��� ������ �� �����
 * ������������� ��. ������ ���� ��� ����� ��������� ������� �� ����������,
 * ������� ���� �� �������������� ������������, ������� �� ���������
 * �������������� (���, ��������, IAR C) ��� ���� ����������� ����������,
 * �������� �� INT0, �� ������ ������ ���-��� �� �����.
 */
/* #define USB_INTR_CFG            MCUCR */
/* #define USB_INTR_CFG_SET        ((1 << ISC00) | (1 << ISC01)) */
/* #define USB_INTR_CFG_CLR        0 */
/* #define USB_INTR_ENABLE         GIMSK */
/* #define USB_INTR_ENABLE_BIT     INT0 */
/* #define USB_INTR_PENDING        GIFR */
/* #define USB_INTR_PENDING_BIT    INTF0 */
/* #define USB_INTR_VECTOR         INT0_vect */

#endif /* __usbconfig_h_included__ */
