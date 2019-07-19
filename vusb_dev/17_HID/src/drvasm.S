#define __SFR_OFFSET 0
#include <avr/io.h>

#include "usbconfig.h"

#define _VECTOR(N)   __vector_ ## N

#define nop2    rjmp    .+0

#define usbTxLen1   usbTxStatus1
#define usbTxBuf1   (usbTxStatus1 + 1)

#define x1      r16
#define x2      r17
#define shift   r18
#define x3      r20
#define cnt     r19
#define x4      r21

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
    lds     YL, usbInputBufOffset;2 [4]
    clr     YH                  ;1 [5]
    subi    YL, lo8(-(usbRxBuf));1 [6]
    sbci    YH, hi8(-(usbRxBuf));1 [7]
    sbis    USBIN, USBMINUS ;1 [8]
      rjmp    haveTwoBitsK    ;2 [10]
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
    cpi     token, USBPID_DATA0 ;[13]
      breq    handleData        ;[14]
    cpi     token, USBPID_DATA1 ;[15]
      breq    handleData        ;[16]
    lds     shift, usbDeviceAddr;[17]
    ldd     x2, y+1             ;[19]
    lsl     x2                  ;[21]
    cpse    x2, shift           ;[22]
      rjmp    ignorePacket      ;[23]
    ldd     x3, y+2             ;[24]
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
    POP_STANDARD                ;[37]
    in YL, USB_INTR_PENDING     ;[49]
    sbrc    YL, USB_INTR_PENDING_BIT;[50]
      rjmp    waitForJ          ;[51]
sofError:
    POP_RETI
    reti
    
handleData:
    lds     shift, usbCurrentTok
    tst     shift
      breq    doReturn
    lds     x2, usbRxLen        ;[22]
    tst     x2                  ;[24]
      brne    sendNakAndReti    ;[25]

    cpi     cnt, 4              ;[26]
      brmi    sendAckAndReti    ;[27]
    sts     usbRxLen, cnt       ;[28]
    sts     usbRxToken, shift   ;[30]
    
    lds     x2, usbInputBufOffset;[32]
    ldi     cnt, USB_BUFSIZE    ;[34]
    sub     cnt, x2             ;[35]
    sts     usbInputBufOffset, cnt;[36]   
      
    rjmp    sendAckAndReti
    
handleIn:
    lds     x1, usbRxLen        ;[30]
    cpi     x1, 1               ;[32]
      brge    sendNakAndReti    ;[33]
    ldi     x1, USBPID_NAK      ;[34]
    
    andi    x3, 0x0F            ;[35]
      brne    handleIn1         ;[36]

    lds     cnt, usbTxLen       ;[36]
    sbrc    cnt, 4              ;[38]
      rjmp    sendCntAndReti    ;[39]
    sts     usbTxLen, x1        ;[40]
    ldi     YL, lo8(usbTxBuf)   ;[42]
    ldi     YH, hi8(usbTxBuf)   ;[43]
    rjmp    usbSendAndReti      ;[44]
    
handleIn1:                      ;[38]
    lds     cnt, usbTxLen1      ;[40]
    sbrc    cnt, 4              ;[42]
      rjmp    sendCntAndReti    ;[43]
    sts     usbTxLen1, x1       ;[44]
    ldi     YL, lo8(usbTxBuf1)  ;[46]
    ldi     YH, hi8(usbTxBuf1)  ;[47]
    rjmp    usbSendAndReti      ;[48]
    
;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
;--- End of Asmcommon.inc --------------------------------------------------
;---------------------------------------------------------------------------
;---------------------------------------------------------------------------
txByteLoop:
txBitloop:
stuffN1Delay:                   ;     [03]
    ror     shift               ;[-5] [11] [59]
      brcc    doExorN1          ;[-4]      [60]
    subi    x4, 1               ;[-3]
      brne    commonN1          ;[-2]
    lsl     shift               ;[-1]
    nop                         ;[00]
    rjmp    stuffN1Delay        ;[01]

sendNakAndReti:                 ;0 [-19]
    ldi     x3, USBPID_NAK      ;1 [-18]
    rjmp    usbSendX3           ;2 [-16]
sendAckAndReti:                 ;0 [-19]
    ldi     x3, USBPID_ACK      ;1 [-18]
    rjmp    usbSendX3           ;2 [-16]
sendCntAndReti:                 ;0 [-17]
    mov     x3, cnt             ;1 [-16]
usbSendX3:                      ;0 [-16]
    ldi     YL, 20              ;1 [-15]
    ldi     YH, 0               ;1 [-14]
    ldi     cnt, 2              ;1 [-13]
;   rjmp    usbSendAndReti      fallthrough

usbSendAndReti:
    in      x2, USBDDR          ;[-12]
    ori     x2, USBMASK         ;[-11]
    sbi     USBOUT, USBMINUS    ;[-10]
    out     USBDDR, x2          ;[-8]
    in      x1, USBOUT          ;[-7]
    
    ldi     shift, 0x40         ;[-6]
    ldi     x2, USBMASK         ;[-5]
    push    x4                  ;[-4]
doExorN1:
    eor     x1, x2              ;[-2] [06] [62]
    ldi     x4, 6               ;[-1] [07] [63]
commonN1:
stuffN2Delay:
    out     USBOUT, x1          ;[00] [08] [64]
    ror     shift               ;[01]
      brcc    doExorN2          ;[02]
    subi    x4, 1               ;[03]
      brne    commonN2          ;[04]
    lsl     shift               ;[05]
    rjmp    stuffN2Delay        ;[06]
doExorN2:
    eor     x1, x2              ;[04] [12]
    ldi     x4, 6               ;[05] [13]
commonN2:
    nop                         ;[06] [14]
    subi    cnt, 171            ;[07] [15]
    out     USBOUT, x1          ;[08] [16]
      brcs    txBitloop         ;[09]      [25] [41]

stuff6Delay:
    ror     shift               ;[42] [50]
      brcc    doExor6           ;[43]
    subi    x4, 1               ;[44]
      brne    common6           ;[45]
    lsl     shift               ;[46]
    nop                         ;[47]
    rjmp    stuff6Delay         ;[48]
doExor6:
    eor     x1, x2              ;[45] [53]
    ldi     x4, 6               ;[46]
common6:
stuff7Delay:
    ror     shift               ;[47] [55]
    out     USBOUT, x1          ;[48]
      brcc    doExor7           ;[49]
    subi    x4, 1               ;[50]
      brne    common7           ;[51]
    lsl     shift               ;[52]
    rjmp    stuff7Delay         ;[53]
doExor7:
    eor     x1, x2              ;[51] [59]
    ldi     x4, 6               ;[52]
common7:
    ld      shift, y+           ;[53]
    tst     cnt                 ;[55]
    out     USBOUT, x1          ;[56]
      brne    txByteLoop        ;[57]

;make SE0:
    cbr     x1, USBMASK         ;[58]
    lds     x2, usbNewDeviceAddr;[59]
    lsl     x2                  ;[61]
    subi    YL, 2 + 20          ;[62]
    sbci    YH, 0               ;[63]
    out     USBOUT, x1          ;[00]
      breq    skipAddrAssign    ;[01]
    sts     usbDeviceAddr, x2
skipAddrAssign:
    ldi     x2, 1<<USB_INTR_PENDING_BIT;[03]
    out     USB_INTR_PENDING, x2;[04]
    ori     x1, USBIDLE         ;[05]
    in      x2, USBDDR          ;[06]
    cbr     x2, USBMASK         ;[07]
    mov     x3, x1              ;[08]
    cbr     x3, USBMASK         ;[09]
    pop     x4                  ;[10]
    nop2                        ;[12]
    nop2                        ;[14]
    out     USBOUT, x1          ;[16]
    out     USBDDR, x2          ;[17]
    out     USBOUT, x3          ;[18]
    rjmp    doReturn

    
;-----------------CRC16---------------------------------------------------------------
#   define argLen   r22 /* argument 2 */
#   define argPtrL  r24 /* argument 1 */
#   define argPtrH  r25 /* argument 1 */

#   define resCrcL  r24 /* result */
#   define resCrcH  r25 /* result */

#   define ptrL     XL
#   define ptrH     XH
#   define ptr      x
#   define byte     r18
#   define bitCnt   r19
#   define polyL    r20
#   define polyH    r21
#   define scratch  r23

usbCrc16:
    mov     ptrL, argPtrL
    mov     ptrH, argPtrH
    ldi     resCrcL, 0
    ldi     resCrcH, 0
    ldi     polyL, lo8(0xa001)
    ldi     polyH, hi8(0xa001)
    com     argLen      ; argLen = -argLen - 1: modified loop to ensure that carry is set
    ldi     bitCnt, 0   ; loop counter with starnd condition = end condition
    rjmp    usbCrcLoopEntry
usbCrcByteLoop:
    ld      byte, ptr+
    eor     resCrcL, byte
usbCrcBitLoop:
    ror     resCrcH     ; carry is always set here (see brcs jumps to here)
    ror     resCrcL
    brcs    usbCrcNoXor
    eor     resCrcL, polyL
    eor     resCrcH, polyH
usbCrcNoXor:
    subi    bitCnt, 224 ; (8 * 224) % 256 = 0; this loop iterates 8 times
    brcs    usbCrcBitLoop
usbCrcLoopEntry:
    subi    argLen, -1
    brcs    usbCrcByteLoop
usbCrcReady:
    ret
    
; extern unsigned usbCrc16Append(unsigned char *data, unsigned char len);
.global usbCrc16
.global usbCrc16Append    
usbCrc16Append:
    rcall   usbCrc16
    st      ptr+, resCrcL
    st      ptr+, resCrcH
    ret

#undef argLen
#undef argPtrL
#undef argPtrH
#undef resCrcL
#undef resCrcH
#undef ptrL
#undef ptrH
#undef ptr
#undef byte
#undef bitCnt
#undef polyL
#undef polyH
#undef scratch
