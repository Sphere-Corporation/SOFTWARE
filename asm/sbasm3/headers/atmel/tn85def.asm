;*************************************************************************
;* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
;* 
;* Number            : AVR000
;* File Name         : "tn85def.asm"
;* Title             : Register/Bit Definitions for the ATtiny85
;* Date              : 2011-02-09
;* Version           : 2.35
;* Support E-mail    : avr@atmel.com
;* Target MCU        : ATtiny85
;* 
;* DESCRIPTION
;* When including this file in the assembly program file, all I/O register 
;* names and I/O register bit names appearing in the data book can be used.
;* In addition, the six registers forming the three data pointers X, Y and 
;* Z have been assigned names XL - ZH. Highest RAM address for Internal 
;* SRAM is also defined 
;* 
;* The Register names are represented by their hexadecimal address.
;* 
;* The Register Bit names are represented by their bit number (0-7).
;* 
;* Please observe the difference in using the bit names with instructions
;* such as "sbr"/"cbr" (set/clear bit in register) and "sbrs"/"sbrc"
;* (skip if bit in register set/cleared). The following example illustrates
;* this:
;* 
;* in    r16,PORTB             ;read PORTB latch
;* sbr   r16,1<<PB6)+1<<PB5) ;set PB6 and PB5 (use masks, not bit#)
;* out   PORTB,r16             ;output to PORTB
;* 
;* in    r16,TIFR              ;read the Timer Interrupt Flag Register
;* sbrc  r16,TOV0              ;test the overflow flag (use bit#)
;* rjmp  TOV0_is_set           ;jump if set
;* ...                         ;otherwise do something else
;*************************************************************************




; ***** SPECIFY DEVICE ***************************************************
; .device ATtiny85

SIGNATURE_000	= 0x1e
SIGNATURE_001	= 0x93
SIGNATURE_002	= 0x0b

                .CR     avr
                .FA     enhanced128k
				.MS		$1000

; ***** I/O REGISTER DEFINITIONS *****************************************
; NOTE:
; Definitions marked "MEMORY MAPPED"are extended I/O ports
; and cannot be used with IN/OUT instructions
SREG	= 0x3f
SPL	= 0x3d
SPH	= 0x3e
GIMSK	= 0x3b
GIFR	= 0x3a
TIMSK	= 0x39
TIFR	= 0x38
SPMCSR	= 0x37
MCUCR	= 0x35
MCUSR	= 0x34
TCCR0B	= 0x33
TCNT0	= 0x32
OSCCAL	= 0x31
TCCR1	= 0x30
TCNT1	= 0x2f
OCR1A	= 0x2e
OCR1C	= 0x2d
GTCCR	= 0x2c
OCR1B	= 0x2b
TCCR0A	= 0x2a
OCR0A	= 0x29
OCR0B	= 0x28
PLLCSR	= 0x27
CLKPR	= 0x26
DT1A	= 0x25
DT1B	= 0x24
DTPS	= 0x23
DWDR	= 0x22
WDTCR	= 0x21
PRR	= 0x20
EEARH	= 0x1f
EEARL	= 0x1e
EEDR	= 0x1d
EECR	= 0x1c
PORTB	= 0x18
DDRB	= 0x17
PINB	= 0x16
PCMSK	= 0x15
DIDR0	= 0x14
GPIOR2	= 0x13
GPIOR1	= 0x12
GPIOR0	= 0x11
USIBR	= 0x10
USIDR	= 0x0f
USISR	= 0x0e
USICR	= 0x0d
ACSR	= 0x08
ADMUX	= 0x07
ADCSRA	= 0x06
ADCH	= 0x05
ADCL	= 0x04
ADCSRB	= 0x03


; ***** BIT DEFINITIONS **************************************************

; ***** PORTB ************************
; PORTB - Data Register, Port B
PORTB0	= 0	; 
PB0	= 0	; For compatibility
PORTB1	= 1	; 
PB1	= 1	; For compatibility
PORTB2	= 2	; 
PB2	= 2	; For compatibility
PORTB3	= 3	; 
PB3	= 3	; For compatibility
PORTB4	= 4	; 
PB4	= 4	; For compatibility
PORTB5	= 5	; 
PB5	= 5	; For compatibility

; DDRB - Data Direction Register, Port B
DDB0	= 0	; 
DDB1	= 1	; 
DDB2	= 2	; 
DDB3	= 3	; 
DDB4	= 4	; 
DDB5	= 5	; 

; PINB - Input Pins, Port B
PINB0	= 0	; 
PINB1	= 1	; 
PINB2	= 2	; 
PINB3	= 3	; 
PINB4	= 4	; 
PINB5	= 5	; 


; ***** ANALOG_COMPARATOR ************
; ADCSRB - ADC Control and Status Register B
ACME	= 6	; Analog Comparator Multiplexer Enable

; ACSR - Analog Comparator Control And Status Register
ACIS0	= 0	; Analog Comparator Interrupt Mode Select bit 0
ACIS1	= 1	; Analog Comparator Interrupt Mode Select bit 1
ACIE	= 3	; Analog Comparator Interrupt Enable
ACI	= 4	; Analog Comparator Interrupt Flag
ACO	= 5	; Analog Compare Output
ACBG	= 6	; Analog Comparator Bandgap Select
AINBG	= ACBG	; For compatibility
ACD	= 7	; Analog Comparator Disable

; DIDR0 - 
AIN0D	= 0	; AIN0 Digital Input Disable
AIN1D	= 1	; AIN1 Digital Input Disable


; ***** AD_CONVERTER *****************
; ADMUX - The ADC multiplexer Selection Register
MUX0	= 0	; Analog Channel and Gain Selection Bits
MUX1	= 1	; Analog Channel and Gain Selection Bits
MUX2	= 2	; Analog Channel and Gain Selection Bits
MUX3	= 3	; Analog Channel and Gain Selection Bits
REFS2	= 4	; Reference Selection Bit 2
ADLAR	= 5	; Left Adjust Result
REFS0	= 6	; Reference Selection Bit 0
REFS1	= 7	; Reference Selection Bit 1

; ADCSRA - The ADC Control and Status register
ADPS0	= 0	; ADC  Prescaler Select Bits
ADPS1	= 1	; ADC  Prescaler Select Bits
ADPS2	= 2	; ADC  Prescaler Select Bits
ADIE	= 3	; ADC Interrupt Enable
ADIF	= 4	; ADC Interrupt Flag
ADATE	= 5	; ADC Auto Trigger Enable
ADSC	= 6	; ADC Start Conversion
ADEN	= 7	; ADC Enable

; ADCH - ADC Data Register High Byte
ADCH0	= 0	; ADC Data Register High Byte Bit 0
ADCH1	= 1	; ADC Data Register High Byte Bit 1
ADCH2	= 2	; ADC Data Register High Byte Bit 2
ADCH3	= 3	; ADC Data Register High Byte Bit 3
ADCH4	= 4	; ADC Data Register High Byte Bit 4
ADCH5	= 5	; ADC Data Register High Byte Bit 5
ADCH6	= 6	; ADC Data Register High Byte Bit 6
ADCH7	= 7	; ADC Data Register High Byte Bit 7

; ADCL - ADC Data Register Low Byte
ADCL0	= 0	; ADC Data Register Low Byte Bit 0
ADCL1	= 1	; ADC Data Register Low Byte Bit 1
ADCL2	= 2	; ADC Data Register Low Byte Bit 2
ADCL3	= 3	; ADC Data Register Low Byte Bit 3
ADCL4	= 4	; ADC Data Register Low Byte Bit 4
ADCL5	= 5	; ADC Data Register Low Byte Bit 5
ADCL6	= 6	; ADC Data Register Low Byte Bit 6
ADCL7	= 7	; ADC Data Register Low Byte Bit 7

; ADCSRB - ADC Control and Status Register B
ADTS0	= 0	; ADC Auto Trigger Source 0
ADTS1	= 1	; ADC Auto Trigger Source 1
ADTS2	= 2	; ADC Auto Trigger Source 2
IPR	= 5	; Input Polarity Mode
BIN	= 7	; Bipolar Input Mode

; DIDR0 - Digital Input Disable Register 0
ADC1D	= 2	; ADC1 Digital input Disable
ADC3D	= 3	; ADC3 Digital input Disable
ADC2D	= 4	; ADC2 Digital input Disable
ADC0D	= 5	; ADC0 Digital input Disable


; ***** USI **************************
; USIBR - USI Buffer Register
USIBR0	= 0	; USI Buffer Register bit 0
USIBR1	= 1	; USI Buffer Register bit 1
USIBR2	= 2	; USI Buffer Register bit 2
USIBR3	= 3	; USI Buffer Register bit 3
USIBR4	= 4	; USI Buffer Register bit 4
USIBR5	= 5	; USI Buffer Register bit 5
USIBR6	= 6	; USI Buffer Register bit 6
USIBR7	= 7	; USI Buffer Register bit 7

; USIDR - USI Data Register
USIDR0	= 0	; USI Data Register bit 0
USIDR1	= 1	; USI Data Register bit 1
USIDR2	= 2	; USI Data Register bit 2
USIDR3	= 3	; USI Data Register bit 3
USIDR4	= 4	; USI Data Register bit 4
USIDR5	= 5	; USI Data Register bit 5
USIDR6	= 6	; USI Data Register bit 6
USIDR7	= 7	; USI Data Register bit 7

; USISR - USI Status Register
USICNT0	= 0	; USI Counter Value Bit 0
USICNT1	= 1	; USI Counter Value Bit 1
USICNT2	= 2	; USI Counter Value Bit 2
USICNT3	= 3	; USI Counter Value Bit 3
USIDC	= 4	; Data Output Collision
USIPF	= 5	; Stop Condition Flag
USIOIF	= 6	; Counter Overflow Interrupt Flag
USISIF	= 7	; Start Condition Interrupt Flag

; USICR - USI Control Register
USITC	= 0	; Toggle Clock Port Pin
USICLK	= 1	; Clock Strobe
USICS0	= 2	; USI Clock Source Select Bit 0
USICS1	= 3	; USI Clock Source Select Bit 1
USIWM0	= 4	; USI Wire Mode Bit 0
USIWM1	= 5	; USI Wire Mode Bit 1
USIOIE	= 6	; Counter Overflow Interrupt Enable
USISIE	= 7	; Start Condition Interrupt Enable


; ***** EXTERNAL_INTERRUPT ***********
; MCUCR - MCU Control Register
ISC00	= 0	; Interrupt Sense Control 0 Bit 0
ISC01	= 1	; Interrupt Sense Control 0 Bit 1

; GIMSK - General Interrupt Mask Register
GICR	= GIMSK	; For compatibility
PCIE	= 5	; Pin Change Interrupt Enable
INT0	= 6	; External Interrupt Request 0 Enable

; GIFR - General Interrupt Flag register
PCIF	= 5	; Pin Change Interrupt Flag
INTF0	= 6	; External Interrupt Flag 0

; PCMSK - Pin Change Enable Mask
PCINT0	= 0	; Pin Change Enable Mask Bit 0
PCINT1	= 1	; Pin Change Enable Mask Bit 1
PCINT2	= 2	; Pin Change Enable Mask Bit 2
PCINT3	= 3	; Pin Change Enable Mask Bit 3
PCINT4	= 4	; Pin Change Enable Mask Bit 4
PCINT5	= 5	; Pin Change Enable Mask Bit 5


; ***** EEPROM ***********************
; EEARL - EEPROM Address Register Low Byte
EEAR0	= 0	; EEPROM Read/Write Access Bit 0
EEAR1	= 1	; EEPROM Read/Write Access Bit 1
EEAR2	= 2	; EEPROM Read/Write Access Bit 2
EEAR3	= 3	; EEPROM Read/Write Access Bit 3
EEAR4	= 4	; EEPROM Read/Write Access Bit 4
EEAR5	= 5	; EEPROM Read/Write Access Bit 5
EEAR6	= 6	; EEPROM Read/Write Access Bit 6
EEAR7	= 7	; EEPROM Read/Write Access Bit 7

; EEARH - EEPROM Address Register High Byte
EEAR8	= 0	; EEPROM Read/Write Access Bit 0

; EEDR - EEPROM Data Register
EEDR0	= 0	; EEPROM Data Register bit 0
EEDR1	= 1	; EEPROM Data Register bit 1
EEDR2	= 2	; EEPROM Data Register bit 2
EEDR3	= 3	; EEPROM Data Register bit 3
EEDR4	= 4	; EEPROM Data Register bit 4
EEDR5	= 5	; EEPROM Data Register bit 5
EEDR6	= 6	; EEPROM Data Register bit 6
EEDR7	= 7	; EEPROM Data Register bit 7

; EECR - EEPROM Control Register
EERE	= 0	; EEPROM Read Enable
EEPE	= 1	; EEPROM Write Enable
EEMPE	= 2	; EEPROM Master Write Enable
EERIE	= 3	; EEPROM Ready Interrupt Enable
EEPM0	= 4	; EEPROM Programming Mode Bit 0
EEPM1	= 5	; EEPROM Programming Mode Bit 1


; ***** WATCHDOG *********************
; WDTCR - Watchdog Timer Control Register
WDTCSR	= WDTCR	; For compatibility
WDP0	= 0	; Watch Dog Timer Prescaler bit 0
WDP1	= 1	; Watch Dog Timer Prescaler bit 1
WDP2	= 2	; Watch Dog Timer Prescaler bit 2
WDE	= 3	; Watch Dog Enable
WDCE	= 4	; Watchdog Change Enable
WDTOE	= WDCE	; For compatibility
WDP3	= 5	; Watchdog Timer Prescaler Bit 3
WDIE	= 6	; Watchdog Timeout Interrupt Enable
WDIF	= 7	; Watchdog Timeout Interrupt Flag


; ***** TIMER_COUNTER_0 **************
; TIMSK - Timer/Counter Interrupt Mask Register
TOIE0	= 1	; Timer/Counter0 Overflow Interrupt Enable
OCIE0B	= 3	; Timer/Counter0 Output Compare Match B Interrupt Enable
OCIE0A	= 4	; Timer/Counter0 Output Compare Match A Interrupt Enable

; TIFR - Timer/Counter0 Interrupt Flag register
TOV0	= 1	; Timer/Counter0 Overflow Flag
OCF0B	= 3	; Timer/Counter0 Output Compare Flag 0B
OCF0A	= 4	; Timer/Counter0 Output Compare Flag 0A

; TCCR0A - Timer/Counter  Control Register A
WGM00	= 0	; Waveform Generation Mode
WGM01	= 1	; Waveform Generation Mode
COM0B0	= 4	; Compare Output Mode, Fast PWm
COM0B1	= 5	; Compare Output Mode, Fast PWm
COM0A0	= 6	; Compare Output Mode, Phase Correct PWM Mode
COM0A1	= 7	; Compare Output Mode, Phase Correct PWM Mode

; TCCR0B - Timer/Counter Control Register B
CS00	= 0	; Clock Select
CS01	= 1	; Clock Select
CS02	= 2	; Clock Select
WGM02	= 3	; 
FOC0B	= 6	; Force Output Compare B
FOC0A	= 7	; Force Output Compare A

; TCNT0 - Timer/Counter0
TCNT0_0	= 0	; 
TCNT0_1	= 1	; 
TCNT0_2	= 2	; 
TCNT0_3	= 3	; 
TCNT0_4	= 4	; 
TCNT0_5	= 5	; 
TCNT0_6	= 6	; 
TCNT0_7	= 7	; 

; OCR0A - Timer/Counter0 Output Compare Register
OCR0_0	= 0	; 
OCR0_1	= 1	; 
OCR0_2	= 2	; 
OCR0_3	= 3	; 
OCR0_4	= 4	; 
OCR0_5	= 5	; 
OCR0_6	= 6	; 
OCR0_7	= 7	; 

; OCR0B - Timer/Counter0 Output Compare Register
;OCR0_0	= 0	; 
;OCR0_1	= 1	; 
;OCR0_2	= 2	; 
;OCR0_3	= 3	; 
;OCR0_4	= 4	; 
;OCR0_5	= 5	; 
;OCR0_6	= 6	; 
;OCR0_7	= 7	; 

; GTCCR - General Timer/Counter Control Register
PSR0	= 0	; Prescaler Reset Timer/Counter1 and Timer/Counter0
TSM	= 7	; Timer/Counter Synchronization Mode


; ***** TIMER_COUNTER_1 **************
; TCCR1 - Timer/Counter Control Register
CS10	= 0	; Clock Select Bits
CS11	= 1	; Clock Select Bits
CS12	= 2	; Clock Select Bits
CS13	= 3	; Clock Select Bits
COM1A0	= 4	; Compare Output Mode, Bit 1
COM1A1	= 5	; Compare Output Mode, Bit 0
PWM1A	= 6	; Pulse Width Modulator Enable
CTC1	= 7	; Clear Timer/Counter on Compare Match

; TCNT1 - Timer/Counter Register
TCNT1_0	= 0	; Timer/Counter Register Bit 0
TCNT1_1	= 1	; Timer/Counter Register Bit 1
TCNT1_2	= 2	; Timer/Counter Register Bit 2
TCNT1_3	= 3	; Timer/Counter Register Bit 3
TCNT1_4	= 4	; Timer/Counter Register Bit 4
TCNT1_5	= 5	; Timer/Counter Register Bit 5
TCNT1_6	= 6	; Timer/Counter Register Bit 6
TCNT1_7	= 7	; Timer/Counter Register Bit 7

; OCR1A - Output Compare Register
OCR1A0	= 0	; Output Compare Register A Bit 0
OCR1A1	= 1	; Output Compare Register A Bit 1
OCR1A2	= 2	; Output Compare Register A Bit 2
OCR1A3	= 3	; Output Compare Register A Bit 3
OCR1A4	= 4	; Output Compare Register A Bit 4
OCR1A5	= 5	; Output Compare Register A Bit 5
OCR1A6	= 6	; Output Compare Register A Bit 6
OCR1A7	= 7	; Output Compare Register A Bit 7

; OCR1B - Output Compare Register
OCR1B0	= 0	; Output Compare Register B Bit 0
OCR1B1	= 1	; Output Compare Register B Bit 1
OCR1B2	= 2	; Output Compare Register B Bit 2
OCR1B3	= 3	; Output Compare Register B Bit 3
OCR1B4	= 4	; Output Compare Register B Bit 4
OCR1B5	= 5	; Output Compare Register B Bit 5
OCR1B6	= 6	; Output Compare Register B Bit 6
OCR1B7	= 7	; Output Compare Register B Bit 7

; OCR1C - Output compare register
OCR1C0	= 0	; 
OCR1C1	= 1	; 
OCR1C2	= 2	; 
OCR1C3	= 3	; 
OCR1C4	= 4	; 
OCR1C5	= 5	; 
OCR1C6	= 6	; 
OCR1C7	= 7	; 

; TIMSK - Timer/Counter Interrupt Mask Register
TOIE1	= 2	; Timer/Counter1 Overflow Interrupt Enable
OCIE1B	= 5	; OCIE1A: Timer/Counter1 Output Compare B Interrupt Enable
OCIE1A	= 6	; OCIE1A: Timer/Counter1 Output Compare Interrupt Enable

; TIFR - Timer/Counter Interrupt Flag Register
TOV1	= 2	; Timer/Counter1 Overflow Flag
OCF1B	= 5	; Timer/Counter1 Output Compare Flag 1B
OCF1A	= 6	; Timer/Counter1 Output Compare Flag 1A

; GTCCR - Timer counter control register
PSR1	= 1	; Prescaler Reset Timer/Counter1
FOC1A	= 2	; Force Output Compare 1A
FOC1B	= 3	; Force Output Compare Match 1B
COM1B0	= 4	; Comparator B Output Mode
COM1B1	= 5	; Comparator B Output Mode
PWM1B	= 6	; Pulse Width Modulator B Enable

; DTPS - Dead time prescaler register
DTPS0	= 0	; 
DTPS1	= 1	; 

; DT1A - Dead time value register
DTVL0	= 0	; 
DTVL1	= 1	; 
DTVL2	= 2	; 
DTVL3	= 3	; 
DTVH0	= 4	; 
DTVH1	= 5	; 
DTVH2	= 6	; 
DTVH3	= 7	; 

; DT1B - Dead time value B
;DTVL0	= 0	; 
;DTVL1	= 1	; 
;DTVL2	= 2	; 
;DTVL3	= 3	; 
;DTVH0	= 4	; 
;DTVH1	= 5	; 
;DTVH2	= 6	; 
;DTVH3	= 7	; 


; ***** BOOT_LOAD ********************
; SPMCSR - Store Program Memory Control Register
SPMEN	= 0	; Store Program Memory Enable
PGERS	= 1	; Page Erase
PGWRT	= 2	; Page Write
RFLB	= 3	; Read fuse and lock bits
CTPB	= 4	; Clear temporary page buffer


; ***** CPU **************************
; SREG - Status Register
SREG_C	= 0	; Carry Flag
SREG_Z	= 1	; Zero Flag
SREG_N	= 2	; Negative Flag
SREG_V	= 3	; Two's Complement Overflow Flag
SREG_S	= 4	; Sign Bit
SREG_H	= 5	; Half Carry Flag
SREG_T	= 6	; Bit Copy Storage
SREG_I	= 7	; Global Interrupt Enable

; MCUCR - MCU Control Register
;ISC00	= 0	; Interrupt Sense Control 0 bit 0
;ISC01	= 1	; Interrupt Sense Control 0 bit 1
SM0	= 3	; Sleep Mode Select Bit 0
SM1	= 4	; Sleep Mode Select Bit 1
SE	= 5	; Sleep Enable
PUD	= 6	; Pull-up Disable

; MCUSR - MCU Status register
PORF	= 0	; Power-On Reset Flag
EXTRF	= 1	; External Reset Flag
BORF	= 2	; Brown-out Reset Flag
WDRF	= 3	; Watchdog Reset Flag

; PRR - Power Reduction Register
PRADC	= 0	; Power Reduction ADC
PRUSI	= 1	; Power Reduction USI
PRTIM0	= 2	; Power Reduction Timer/Counter0
PRTIM1	= 3	; Power Reduction Timer/Counter1

; OSCCAL - Oscillator Calibration Register
CAL0	= 0	; Oscillatro Calibration Value Bit 0
CAL1	= 1	; Oscillatro Calibration Value Bit 1
CAL2	= 2	; Oscillatro Calibration Value Bit 2
CAL3	= 3	; Oscillatro Calibration Value Bit 3
CAL4	= 4	; Oscillatro Calibration Value Bit 4
CAL5	= 5	; Oscillatro Calibration Value Bit 5
CAL6	= 6	; Oscillatro Calibration Value Bit 6
CAL7	= 7	; Oscillatro Calibration Value Bit 7

; PLLCSR - PLL Control and status register
PLOCK	= 0	; PLL Lock detector
PLLE	= 1	; PLL Enable
PCKE	= 2	; PCK Enable
LSM	= 7	; Low speed mode

; CLKPR - Clock Prescale Register
CLKPS0	= 0	; Clock Prescaler Select Bit 0
CLKPS1	= 1	; Clock Prescaler Select Bit 1
CLKPS2	= 2	; Clock Prescaler Select Bit 2
CLKPS3	= 3	; Clock Prescaler Select Bit 3
CLKPCE	= 7	; Clock Prescaler Change Enable

; DWDR - debugWire data register
DWDR0	= 0	; 
DWDR1	= 1	; 
DWDR2	= 2	; 
DWDR3	= 3	; 
DWDR4	= 4	; 
DWDR5	= 5	; 
DWDR6	= 6	; 
DWDR7	= 7	; 

; GPIOR2 - General Purpose IO register 2
GPIOR20	= 0	; 
GPIOR21	= 1	; 
GPIOR22	= 2	; 
GPIOR23	= 3	; 
GPIOR24	= 4	; 
GPIOR25	= 5	; 
GPIOR26	= 6	; 
GPIOR27	= 7	; 

; GPIOR1 - General Purpose register 1
GPIOR10	= 0	; 
GPIOR11	= 1	; 
GPIOR12	= 2	; 
GPIOR13	= 3	; 
GPIOR14	= 4	; 
GPIOR15	= 5	; 
GPIOR16	= 6	; 
GPIOR17	= 7	; 

; GPIOR0 - General purpose register 0
GPIOR00	= 0	; 
GPIOR01	= 1	; 
GPIOR02	= 2	; 
GPIOR03	= 3	; 
GPIOR04	= 4	; 
GPIOR05	= 5	; 
GPIOR06	= 6	; 
GPIOR07	= 7	; 



; ***** LOCKSBITS ********************************************************
LB1	= 0	; Lockbit
LB2	= 1	; Lockbit


; ***** FUSES ************************************************************
; LOW fuse bits
CKSEL0	= 0	; Select Clock source
CKSEL1	= 1	; Select Clock source
CKSEL2	= 2	; Select Clock source
CKSEL3	= 3	; Select Clock source
SUT0	= 4	; Select start-up time
SUT1	= 5	; Select start-up time
CKOUT	= 6	; Clock Output Enable
CKDIV8	= 7	; Divide clock by 8

; HIGH fuse bits
BODLEVEL0	= 0	; Brown-out Detector trigger level
BODLEVEL1	= 1	; Brown-out Detector trigger level
BODLEVEL2	= 2	; Brown-out Detector trigger level
EESAVE	= 3	; EEPROM memory is preserved through the Chip Erase
WDTON	= 4	; Watchdog Timer always on
SPIEN	= 5	; Enable Serial Program and Data Downloading
DWEN	= 6	; DebugWIRE Enable
RSTDISBL	= 7	; External Reset disable

; EXTENDED fuse bits
SELFPRGEN	= 0	; Self-Programming Enable



; ***** CPU REGISTER DEFINITIONS *****************************************
XH	.DE r27
XL	.DE r26
YH	.DE r29
YL	.DE r28
ZH	.DE r31
ZL	.DE r30



; ***** DATA MEMORY DECLARATIONS *****************************************
FLASHEND	= 0x0fff	; Note: Word address
IOEND	= 0x003f
SRAM_START	= 0x0060
SRAM_SIZE	= 512
RAMEND	= 0x025f
XRAMEND	= 0x0000
E2END	= 0x01ff
EEPROMEND	= 0x01ff
EEADRBITS	= 9



; ***** BOOTLOADER DECLARATIONS ******************************************
NRWW_START_ADDR	= 0x0
NRWW_STOP_ADDR	= 0xfff
RWW_START_ADDR	= 0x0
RWW_STOP_ADDR	= 0x0
PAGESIZE	= 32



; ***** INTERRUPT VECTORS ************************************************
INT0addr	= 0x0001	; External Interrupt 0
PCI0addr	= 0x0002	; Pin change Interrupt Request 0
OC1Aaddr	= 0x0003	; Timer/Counter1 Compare Match 1A
OVF1addr	= 0x0004	; Timer/Counter1 Overflow
OVF0addr	= 0x0005	; Timer/Counter0 Overflow
ERDYaddr	= 0x0006	; EEPROM Ready
ACIaddr	= 0x0007	; Analog comparator
ADCCaddr	= 0x0008	; ADC Conversion ready
OC1Baddr	= 0x0009	; Timer/Counter1 Compare Match B
OC0Aaddr	= 0x000a	; Timer/Counter0 Compare Match A
OC0Baddr	= 0x000b	; Timer/Counter0 Compare Match B
WDTaddr	= 0x000c	; Watchdog Time-out
USI_STARTaddr	= 0x000d	; USI START
USI_OVFaddr	= 0x000e	; USI Overflow

INT_VECTORS_SIZE	= 15	; size in words


; ***** END OF FILE ******************************************************
