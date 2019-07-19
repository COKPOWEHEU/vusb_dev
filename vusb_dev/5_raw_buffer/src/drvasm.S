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
    nop                     ;1 [18 / 2]
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
    
rxLoop:
    eor     shift, x3       ;1 [0]
    in      x1, USBIN       ;1 [1]
    st      y+, shift       ;2 [3]
    nop                     ;1 [4]
    nop                     ;1 [5]
    eor     x2, x1          ;1 [6]
    bst     x2, USBMINUS    ;1 [7]
    bld     shift, 0        ;1 [8]
    in      x2, USBIN       ;1 [9]
    andi    x2, USBMASK     ;1 [10]
      breq    se0           ;1 [11]
    nop                     ;1 [12]
    nop                     ;1 [13]
    eor     x1, x2          ;1 [14]
    bst     x1, USBMINUS    ;1 [15]
    bld     shift, 1        ;1 [16]
rxbit2:
    in      x1, USBIN       ;1 [17]
    nop                     ;1 [18]
    nop                     ;1 [19]
    
    subi    cnt, 1          ;1 [20]
      brcs  overflow        ;1 [21]
    eor     x2, x1          ;1 [22]
    bst     x2, USBMINUS    ;1 [23]
    bld     shift, 2        ;1 [24]
    in      x2, USBIN       ;1 [25]
    nop                     ;1 [26]
    nop                     ;1 [27]
    eor     x1, x2          ;1 [28]
    bst     x1, USBMINUS    ;1 [29]
    bld     shift, 3        ;1 [30]
    nop                     ;1 [31]
    nop                     ;1 [32]
    in      x1, USBIN       ;1 [33]
    eor     x2, x1          ;1 [34]
    bst     x2, USBMINUS    ;1 [35]
    bld     shift, 4        ;1 [36]
    nop                     ;1 [37]
    nop                     ;1 [38]
    nop2                    ;2 [40]
    in      x2, USBIN       ;1 [41]
    eor     x1, x2          ;1 [42]
    bst     x1, USBMINUS    ;1 [43]
    bld     shift, 5        ;1 [44]
    nop                     ;1 [45]
    nop                     ;1 [46]
    nop2                    ;2 [48]
    in      x1, USBIN       ;1 [49]
    eor     x2, x1          ;1 [50]
    bst     x2, USBMINUS    ;1 [51]
    bld     shift, 6        ;1 [52]
    nop                     ;1 [53]
    nop                     ;1 [54]
    nop2                    ;2 [56]
    in      x2, USBIN       ;1 [57]
    eor     x1, x2          ;1 [58]
    bst     x1, USBMINUS    ;1 [59]
    bld     shift, 7        ;1 [60]
      
    nop                     ;1 [61]
    rjmp rxLoop             ;3 [63]
    
overflow:
    ldi x2, 0xFF
se0:
wait_se0:
    sbic USBIN, USBMINUS
      ldi x2, 0xFF
    sbic USBIN, USBPLUS
      ldi x2, 0xFF
    dec x2
      brne wait_se0
      
      
    push temp
    subi YL, lo8(usbRxBuf)
    mov cnt, YL   ;вычисляем количество записанных байтов
    ldi temp, '>' ;добавляем символ разделителя
    out UDR, temp
    mov temp, cnt
    rcall uart_hex
    ldi YH, hi8(usbRxBuf);
    ldi YL, lo8(usbRxBuf);
loop_out:
    ld temp, Y+
    rcall uart_hex
    dec cnt
    brne loop_out
    
    pop temp
    
    pop cnt
    pop x3
    pop x2
    pop x1
    pop shift
    pop YH
    pop YL
    out SREG, YL
    pop YL
reti

sofError:
    pop YL
    out SREG, YL
    pop YL
reti



uart_hex:
  push x2
  mov x2, temp
  swap temp
  andi temp, 0x0F
  subi temp, -('0')
  cpi temp, ('9'+1)
    brlo temp_10
  subi temp, -('A'-'0'-0x0A)
temp_10:
    andi x2, 0x0F
    subi x2, -('0')
    cpi x2, ('9'+1)
      brlo x2_10
    subi x2, -('A'-'0'-0x0A)
x2_10:
    sbis UCSRA, UDRE
      rjmp x2_10
    out UDR, temp
    nop
uart_2:
    sbis UCSRA, UDRE
      rjmp uart_2
    out UDR, x2
    nop
uart_3:
    sbis UCSRA, UDRE
      rjmp uart_3
    ldi x2, ' '
    out UDR, x2
  pop x2
ret
