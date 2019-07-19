#define __SFR_OFFSET 0      /* used by avr-libc's register definitions */
#include <avr/io.h>

#include "usbconfig.h"

#define _VECTOR(N)   __vector_ ## N   /* io.h does not define this for asm */

#define x1      r16
#define x2      r17
#define shift   r18
#define raw     r19
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
    sbis    USBIN, USBMINUS ;если DM=1 (J-state) goto waitForK
      brne    waitForJ ;если DM=0 (K-state) продолжаем ждать. Но если таймаут кончился, идем в waitForK 
waitForK:
    sbis    USBIN, USBMINUS ;если DM=0 (K-state) goto foundK
      rjmp    foundK
    sbis    USBIN, USBMINUS
      rjmp    foundK
    sbis    USBIN, USBMINUS
      rjmp    foundK
    sbis    USBIN, USBMINUS
      rjmp    foundK
    sbis    USBIN, USBMINUS
      rjmp    foundK
    rjmp    sofError ;если все еще DM=1 (J-state) то ошибка
foundK: ;сюда мы попали в середине К-состояния, ждем середину следующего бита - будет ли он K или J
    nop; 1 [1]
    nop; 1 [2]
    nop; 1 [3]
    nop; 1 [4]
    nop; 1 [5]
    nop; 1 [6]
    nop; 1 [7]

    sbis    USBIN, USBMINUS ;1 [8] нам нужно два K-бита подряд как флаг конца байта sync
      rjmp    haveTwoBitsK  ;2 [10]
    nop                     ;1 [10]
    nop                     ;1 [11]
    rjmp    waitForK        ;2 [13] если поймали очередной J, возвращаемся к ожиданию двойного K-бита
haveTwoBitsK:
    push shift              ;2 [12]
    push x1                 ;2 [14]
    push x2                 ;2 [16]
    ;push raw      эта операция записана ниже, "между строчек"
    
    in x1, USBIN    ;1 [17 = 0 / 0] <---------------
    ldi shift, 0xFF ;1 [18 = 1 / 0]
    bst x1, USBMINUS;1 [19 = 2 / 0]
    bld shift, 7    ;1 [20 = 3 / 0]
    ldi x2, 7       ;1 [21 = 4 / 0]
    push raw        ;2 [23 = 6 / 0]
    nop             ;1 [24 = 7 / 0]

    
loop_cmd:
    in x1, USBIN    ;1 [0 / i] <---------------
    lsr shift       ;1 [1 / i]
    bst x1, USBMINUS;1 [2 / i]
    bld shift, 7    ;1 [3 / i]
    nop             ;1 [4 / i]
    dec x2          ;1 [5 / i]
      brne loop_cmd ;2 [7 / i]
      
    
    ldi x2, 8
loop_data:
    in x1, USBIN    ;1 [0 / i] <---------------
    lsr raw         ;1 [1 / i]
    bst x1, USBMINUS;1 [2 / i]
    bld raw, 7      ;1 [3 / i]
    nop             ;1 [4 / i]
    dec x2          ;1 [5 / i]
      brne loop_data;2 [7 / i]
  
;ждем состояния SE0, когда на D+ и D- одновременно низкий уровень
    ldi x2, 0xFF
wait_se0:
    sbic USBIN, USBMINUS
      ldi x2, 0xFF
    sbic USBIN, USBPLUS
      ldi x2, 0xFF
    dec x2
      brne wait_se0
    
;отправляем полученные байты
    push temp
    mov temp, shift
    rcall uart_hex
    mov temp, raw
    rcall uart_hex
    pop temp
    
    pop raw
    pop x2
    pop x1
    pop shift
    pop YL
    out SREG, YL
    pop YL
reti

sofError:
    pop YL
    out SREG, YL
    pop YL
reti


;передача байта по UART в шестнадцатеричном формате
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
