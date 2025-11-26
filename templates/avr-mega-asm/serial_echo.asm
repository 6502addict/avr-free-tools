; serial_echo.asm
; Serial Echo - AVR Assembler
; Receives data on UART0 and echoes it back
; Compatible with ATmega328P, ATmega2560, ATmega1284P


.IFDEF ATmega328P
.INCLUDE "m328Pdef.inc"
.ENDIF			
.IFDEF ATmega2560
.INCLUDE "m2560def.inc"
.ENDIF			
.IFDEF ATmega1284P
.INCLUDE "m1284Pdef.inc"
.ENDIF			
.IFDEF ATtiny85
.INCLUDE "t85def.inc"
.ENDIF

; Calculate baud rate register value
; UBRR = (F_CPU / (16 * BAUD)) - 1
.equ UBRR_VALUE = (F_CPU / (16 * BAUD)) - 1

.org 0x0000
    rjmp main           ; Reset vector

main:
    ; Initialize stack pointer
    ldi r16, low(RAMEND)
    out SPL, r16
    ldi r16, high(RAMEND)
    out SPH, r16

    ; Initialize UART
    rcall uart_init

    ; Main loop - echo received data
loop:
    rcall uart_getc     ; Receive byte in r24
    rcall uart_putc     ; Transmit byte from r24
    rjmp loop

; UART initialization
uart_init:
    ; Set baud rate
    ldi r16, high(UBRR_VALUE)
    sts UBRR0H, r16
    ldi r16, low(UBRR_VALUE)
    sts UBRR0L, r16

    ; Enable receiver and transmitter
    ldi r16, (1<<RXEN0) | (1<<TXEN0)
    sts UCSR0B, r16

    ; Set frame format: 8 data bits, 1 stop bit, no parity
    ldi r16, (1<<UCSZ01) | (1<<UCSZ00)
    sts UCSR0C, r16

    ret

; Transmit single byte
; Input: r24 = byte to transmit
uart_putc:
    push r16
uart_putc_wait:
    ; Wait for empty transmit buffer
    lds r16, UCSR0A
    sbrs r16, UDRE0
    rjmp uart_putc_wait

    ; Put data into buffer
    sts UDR0, r24
    pop r16
    ret

; Receive single byte
; Output: r24 = received byte
uart_getc:
    push r16
uart_getc_wait:
    ; Wait for data to be received
    lds r16, UCSR0A
    sbrs r16, RXC0
    rjmp uart_getc_wait

    ; Get received data from buffer
    lds r24, UDR0
    pop r16
    ret
