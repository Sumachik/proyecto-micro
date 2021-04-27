#define CNTRL_PT PORTB
#define CNTRL_TS TRISB
#define DATA_PT PORTD
#define	DATA_TS TRISD
	;; For 20Mhz use 12.     Can be reduced at lower clock speeds
	;; For 12.288Mhz use 5   Select on test !
#define DELAY 12	
	;; Definitions for bits in CNTRL_PT 
#define CSA 0
#define CSB 1
#define RS 	2
#define RW  3
#define EN  4
#define RST 5
	;; Definiciones del Registro de estado
#define	BUSY	7
#define	ON_OFF	5
#define	RESET	4
	
	CBLOCK
		var1
		var2
		var3
		tempD
		tempC
		TEMP
		CHAR
	ENDC
SETA
		bcf		CNTRL_PT,CSA
		bsf		CNTRL_PT,CSB
		return
	
SETB	
		bsf		CNTRL_PT,CSA
		bcf		CNTRL_PT,CSB
		return
SETBOTH 
		bsf		CNTRL_PT,CSA
		bsf		CNTRL_PT,CSB
		return
;===================================================================
initGRLCD:		
	banksel	DATA_TS
	clrf	DATA_TS			;DATA_PT como salida
	clrf	CNTRL_TS		;CNTRL_PT como salida
	banksel	DATA_PT	
	bcf		CNTRL_PT,RST	; 5      XX0XXXXX
	bcf		CNTRL_PT,CSA	; 0      XX0XXXX0
	bcf		CNTRL_PT,CSB	; 1      XX0XXX00
	bcf		CNTRL_PT,EN 	; 4      XX00XX00
	bcf		CNTRL_PT,RS		; 2      XX00X000
	bcf		CNTRL_PT,RW		; 3      XX000000
	bsf		CNTRL_PT,RST	;        XX100000
	bcf		CNTRL_PT,CSA
	bsf		CNTRL_PT,CSB
	movlw	0x3F		; Display ON
	call	EnviaCmdGLCD	
	movlw	0x40		; set Y address to 0
	call	SetY
	movlw	0xB8		;  set X page to 0 
	call	SetX
	movlw	0xC0		;  set display start line to 0
	call	SetLS
	bsf		CNTRL_PT,CSA
	bcf		CNTRL_PT,CSB
	movlw	0x3F		; Display ON
	call	EnviaCmdGLCD	
	movlw	0x40		; set Y address to 0
	call	SetY
	movlw	0xB8		;  set X page to 0 
	call	SetX
	movlw	0xC0		;  set display start line to 0
	call	SetLS
	bsf		CNTRL_PT,CSB
	return
;===================================================================
TestBusyGLCD
	;movwf	W_TEMP
	banksel	DATA_TS
	movlw	0xFF			;Seleccionar DATA_PT como entrada
	movwf	DATA_TS
	banksel	DATA_PT
	bcf     CNTRL_PT,RS        ;Porne LCD en modo comando
	bsf     CNTRL_PT,RW        ;prepara para leer flag de busy
	call	STROBE_EN
	btfsc   DATA_PT,BUSY       ;Chequea flag de busy, alto = ocupado
	goto    TestBusyGLCD     ;Si esta ocupado, chequear nuevamente
	bcf     CNTRL_PT,RW        
	banksel	DATA_TS
	clrf	DATA_TS			;DATA_PT como salida
	banksel	DATA_PT
	;movf	W_TEMP,W
	return	
;===================================================================
	;; Clear the LCD
ClearGLCD:
	clrf	var1
	;bsf		CNTRL_PT,CSA
	;bsf		CNTRL_PT,CSB
ClearLoop1
	mov		var2,.64
	movf	var1,W
	call	SetX	
	movlw	.0
	call	SetY
	movlw	.0
	call	SetLS
ClearLoop2	
	movlw	0x00
	call	EnviaCarGLCD
	decfsz	var2,F
	goto	ClearLoop2
	incf	var1,F
	btfss	var1,3
	goto	ClearLoop1
	return
;===================================================================
EnviaCarGLCD
	movwf	W_TEMP			;almacena temporal W
	call    TestBusyGLCD     ;Esperar que LCD este listo
	movf    W_TEMP,W          
	movwf   DATA_PT         ;Enviar datos al LCD
	bcf     CNTRL_PT,RW     ;Poner al LCD en modo escritura
	bsf     CNTRL_PT,RS     ;Poner al LCD en modo de datos
	call	STROBE_EN
	return
;===================================================================
EnviaCmdGLCD
	movwf   W_TEMP          ;Caracter a ser enviado esta en reg. W
	call    TestBusyGLCD    ;esperar LCD listo
	movf	W_TEMP,W
	movwf   DATA_PT         ;enviar dato al LCD
	bcf     CNTRL_PT,RW     ;Poner LCD en modo escritura
	bcf     CNTRL_PT,RS     ;Poner LCD en modo comando
	call	STROBE_EN
	return	
;===================================================================
SetY
	andlw	b'01111111'
	iorlw	b'01000000'
	movwf	W_TEMP
	call	TestBusyGLCD
	movf	W_TEMP,W
	movwf	DATA_PT
	banksel	CNTRL_PT
	bcf		CNTRL_PT,RS
	bcf		CNTRL_PT,RW
	call	STROBE_EN
	return
;===================================================================	
SetX
	andlw	b'10111111'
	iorlw	b'10111000'
	movwf	W_TEMP
	call	TestBusyGLCD
	movf	W_TEMP,W
	movwf	DATA_PT
	banksel	CNTRL_PT
	bcf		CNTRL_PT,RS
	bcf		CNTRL_PT,RW
	call	STROBE_EN
	return
;===================================================================	
SetLS
	andlw	b'11111111'
	iorlw	b'11000000'
	movwf	W_TEMP
	call	TestBusyGLCD
	movf	W_TEMP,W
	movwf	DATA_PT
	banksel	CNTRL_PT
	bcf		CNTRL_PT,RS
	bcf		CNTRL_PT,RW
	call	STROBE_EN
	return
;===================================================================	
STROBE_EN
	banksel	CNTRL_PT
	nop
	bsf		CNTRL_PT,EN
	nop
	nop
	nop
	bcf		CNTRL_PT,EN
	nop
	nop
	nop
	return