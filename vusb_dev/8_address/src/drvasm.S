#define __SFR_OFFSET 0
#include <avr/io.h>

#include "usbconfig.h"

#define _VECTOR(N)   __vector_ ## N

#define nop2    rjmp    .+0

#define x1      r16
#define x2      r17
#define shift   r18
#define x3      r20
#define cnt     r19
#define temp    r20

.text
.global INT0_vect
.type   INT0_vect, @function
INT0_vect:
    push    YL
    in      YL, SREG
    push    YL
    
waitForJ:
    inc     YL
    sbis    USBIN, USBMINUS
      brne    waitForJ
waitForK:
    sbis    USBIN, USBMINUS
      rjmp    foundK
    sbis    USBIN, USBMINUS
      rjmp    foundK
    sbis    USBIN, USBMINUS
      rjmp    foundK
    sbis    USBIN, USBMINUS
      rjmp    foundK
    sbis    USBIN, USBMINUS
      rjmp    foundK
    rjmp    sofError
foundK:
    push YH                 ;2 [2]
    ldi  YH, hi8(usbRxBuf)  ;1 [3]
    ldi  YL, lo8(usbRxBuf)  ;1 [4]
    nop                     ;1 [5]
    nop                     ;1 [6]
    nop                     ;1 [7]

    sbis    USBIN, USBMINUS ;1 [8]
      rjmp    haveTwoBitsK  ;2 [10]
    pop YH                  ;2 [12]
    rjmp    waitForK        ;2 [13]
haveTwoBitsK:
    push shift              ;2 [12]
    push x1                 ;2 [14]
    push x2                 ;2 [16]
    ;push x3
    ;push cnt
    
    in      x1, USBIN       ;1 [17 / 1]
    ldi     shift, 0xFF     ;1 [18 / 2]
    bst     x1, USBMINUS    ;1 [19 / 3]
    bld     shift, 0        ;1 [20 / 4]
    push    x3              ;2 [22 / 6]
    push    cnt             ;2 [24 / 8]
    
    in      x2, USBIN       ;1 [25 / 9]
    ldi     x3, 0xFF        ;1 [26 / 10]
    eor     x1, x2          ;1 [27 / 11]
    bst     x1, USBMINUS    ;1 [28 / 12]
    bld     shift, 1        ;1 [29 / 13]
    ldi     cnt, USB_BUFSIZE;1 [30 / 14]
    rjmp    rxbit2          ;1 [32 / 16]

    
unstuff0:                   ;1
    andi    x3, ~(1<<0)     ;1 [15]
    mov     x1, x2          ;1 [16]
    in      x2, USBIN       ;1 [17]
    ori     shift, (1<<0)   ;1 [18]
    rjmp    didUnstuff0     ;2 [20]

unstuff1:                   ;1
    mov     x2, x1          ;1 [21]
    andi    x3, ~(1<<1)     ;1 [22]
    ori     shift, (1<<1)   ;1 [23]
    nop                     ;1 [24]
    in      x1, USBIN       ;1 [25]
    rjmp    didUnstuff1     ;2 [27]

unstuff2:                   ;1
    andi    x3, ~(1<<2)     ;1 [29]
    ori     shift, (1<<2)   ;1 [30]
    mov     x1, x2          ;1 [31]
    nop                     ;1 [32]
    in      x2, USBIN       ;1 [33]
    rjmp    didUnstuff2     ;2 [35]

unstuff3:                   ;1
    in      x2, USBIN       ;1 [34]
    andi    x3, ~(1<<3)     ;1 [35]
    ori     shift, (1<<3)   ;1 [36]
    rjmp    didUnstuff3     ;2 [38]

unstuff4:                   ;1
    andi    x3, ~(1<<4)     ;1 [40]
    in      x1, USBIN       ;1 [41]
    ori     shift, (1<<4)   ;1 [42]
    rjmp    didUnstuff4     ;2 [44]

unstuff5:                   ;1
    andi    x3, ~(1<<5)     ;1 [48]
    in      x2, USBIN       ;1 [49]
    ori     shift, (1<<5)   ;1 [50]
    rjmp    didUnstuff5     ;2 [52]

unstuff6:                   ;1
    andi    x3, ~(1<<6)     ;1 [56]
    in      x1, USBIN       ;1 [57]
    ori     shift, (1<<6)   ;1 [58]
    rjmp    didUnstuff6     ;2 [60]
rxLoop:
    eor     x3, shift       ;1 [0]
    in      x1, USBIN       ;1 [1]
    st      y+, x3          ;2 [3]
    ldi     x3, 0xFF        ;1 [4]
    nop                     ;1 [5]
    eor     x2, x1          ;1 [6]
    bst     x2, USBMINUS    ;1 [7]
    bld     shift, 0        ;1 [8]
    in      x2, USBIN       ;1 [9]
    andi    x2, USBMASK     ;1 [10]
      breq  se0             ;1 [11]
    andi    shift, 0xF9     ;1 [12]
didUnstuff0:
      breq  unstuff0        ;1 [13]
    eor     x1, x2          ;1 [14]
    bst     x1, USBMINUS    ;1 [15]
    bld     shift, 1        ;1 [16]
rxbit2:
    in      x1, USBIN       ;1 [17]
    andi    shift, 0xF3     ;1 [18]
      breq  unstuff1        ;1 [19]
didUnstuff1:    
    subi    cnt, 1          ;1 [20]
      brcs  overflow        ;1 [21]
    eor     x2, x1          ;1 [22]
    bst     x2, USBMINUS    ;1 [23]
    bld     shift, 2        ;1 [24]
    in      x2, USBIN       ;1 [25]
    andi    shift, 0xE7     ;1 [26]
      breq  unstuff2        ;1 [27]
didUnstuff2:
    eor     x1, x2          ;1 [28]
    bst     x1, USBMINUS    ;1 [29]
    bld     shift, 3        ;1 [30]
didUnstuff3:
    andi    shift, 0xCF     ;1 [31]
      breq  unstuff3        ;1 [32]
    in      x1, USBIN       ;1 [33]
    eor     x2, x1          ;1 [34]
    bst     x2, USBMINUS    ;1 [35]
    bld     shift, 4        ;1 [36]
didUnstuff4:
    andi    shift, 0x9F     ;1 [37]
      breq  unstuff4        ;1 [38]
    nop2                    ;2 [40]
    in      x2, USBIN       ;1 [41]
    eor     x1, x2          ;1 [42]
    bst     x1, USBMINUS    ;1 [43]
    bld     shift, 5        ;1 [44]
didUnstuff5:
    andi    shift, 0x3F     ;1 [45]
      breq  unstuff5        ;1 [46]
    nop2                    ;2 [48]
    in      x1, USBIN       ;1 [49]
    eor     x2, x1          ;1 [50]
    bst     x2, USBMINUS    ;1 [51]
    bld     shift, 6        ;1 [52]
didUnstuff6:
    cpi     shift, 0x02     ;1 [53]
      brlo  unstuff6        ;1 [54]
    nop2                    ;2 [56]
    in      x2, USBIN       ;1 [57]
    eor     x1, x2          ;1 [58]
    bst     x1, USBMINUS    ;1 [59]
    bld     shift, 7        ;1 [60]
didUnstuff7:
    cpi     shift, 0x04     ;1 [61]
      brsh  rxLoop          ;3 [63]
unstuff7:
    andi    x3, ~(1<<7)     ;1 [63]
    ori     shift, (1<<7)   ;1 [64]
    in      x2, USBIN       ;1 [65]
    nop                     ;1 [66]
    rjmp    didUnstuff7     ;2 [68]

    
.macro POP_STANDARD ; 12 cycles
    pop     cnt
    pop     x3
    pop     x2
    pop     x1
    pop     shift
    pop     YH
    .endm
.macro POP_RETI     ; 5 cycles
    pop     YL
    out     SREG, YL
    pop     YL
    .endm     

;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
;--- Asmcommon.inc ---------------------------------------------------------
;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
#define token   x1

overflow:
    ldi     x2, 1<<USB_INTR_PENDING_BIT
    out USB_INTR_PENDING, x2
ignorePacket:
    clr     token
    rjmp    storeTokenAndReturn
se0:
    subi    cnt, USB_BUFSIZE    ;[5]
    neg     cnt                 ;[6]
    sub     YL, cnt             ;[7]
    sbci    YH, 0               ;[8]
    ldi     x2, 1<<USB_INTR_PENDING_BIT ;[9]
    out USB_INTR_PENDING, x2    ;[10]
    ld      token, y            ;[11]
    ;пакеты данных не содержат адреса, поэтому игнорируем
    cpi     token, USBPID_DATA0 ;[13]
      breq    handleData        ;[14]
    cpi     token, USBPID_DATA1 ;[15]
      breq    handleData        ;[16]
    ;вычисляем адрес устройства и адрес конечной точки
    lds     shift, usbDeviceAddr;[17]
    ldd     x2, y+1             ;[19]
    lsl     x2                  ;[21]
    cpse    x2, shift           ;[22]
      rjmp    ignorePacket      ;[23]
    ldd     x3, y+2             ;[24] формируем номер конечной точки
    rol     x3                  ;[26]
    cpi     token, USBPID_IN    ;[27]
      breq    handleIn          ;[28]
    cpi     token, USBPID_SETUP ;[29]
      breq    handleSetupOrOut  ;[30]
    cpi     token, USBPID_OUT   ;[31]
      brne    ignorePacket      ;[32]
;   rjmp    handleSetupOrOut    ; fallthrough

handleSetupOrOut:               ;[32]
storeTokenAndReturn:
    sts     usbCurrentTok, token;[35]
doReturn:
    POP_STANDARD                ;[37] 12...16 cycles
    in YL, USB_INTR_PENDING     ;[49]
    sbrc    YL, USB_INTR_PENDING_BIT;[50]
      rjmp    waitForJ          ;[51]
sofError:
    POP_RETI
    reti
    
handleData:
  sbi RLED_PORT, RLED_BIT
  rjmp ignorePacket
handleIn:
  sbi GLED_PORT, GLED_BIT
  rjmp ignorePacket

;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
;--- End of Asmcommon.inc --------------------------------------------------
;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
