***************************
; UFSC- Universidade Federal de Santa Catarina
; Projeto: Exerc�cio - Carro de Transporte
; Professor: Roderval Marcelino
; Nomes: Cassiano Matias Luzzietti & Jos� Norberto Fagundes Isa�as
;***************************

#include <P16F877A.INC> 	; Arquivo de include do uC usado, no caso PIC16F877A

; Palavra de configura��o, desabilita e habilita algumas op��es internas
  __CONFIG  _CP_OFF & _CPD_OFF & _DEBUG_OFF & _LVP_OFF & _WRT_OFF & _BODEN_OFF & _PWRTE_OFF & _WDT_OFF & _XT_OSC

  
;** defini��o de Vari�veis  **********

  	cblock 0x20
		tempo0		
		tempo1		; Vari�veis usadas na rotina de delay.
		tempo2
        					
	endc 
	
 ;*** Vetor de Reset do uC*****
 org 0x00			; Vetor de reset do uC.
 goto inicio			; Desvia para o inicio do programa.

;***** Rotinas e Sub-Rotinas ***************
; Rotina de delay (De 1ms at� 256 ms)
delay_ms:
	    movlw	.1	; vai carrega tempo2 com constante  
	    movwf	tempo2	; carrega tempo2   
denovo2:    movlw	.250	; vai carrega tempo1 com constante
	    movwf	tempo1	; Carrega tempo1 
denovo1:    movlw	.250	; vai carregar tempo0 com constante
	    movwf	tempo0	; Carrega tempo0 
volta:	    nop			; gasta 1 ciclo de m?quina(1us para clock 4Mhz)
	    decfsz	tempo0,F; Fim de tempo0 ? (gasta 2 us)
	    goto	volta	; N�o - Volta (gasta 1us)
				; Sim - Passou-se 1ms. (6us x 250 = 1ms)
	    decfsz	tempo1,F; Fim de tempo1?
	    goto	denovo1	; N�o - Volta 
				; Sim. 250 x 1ms = 1500ms	
	    decfsz	tempo2,F; Fim de tempo2?
	    goto	denovo2	; N�o - Volta 
				; Sim. 6 x 250 = 1,5s				
 return   		    	; Retorna.

;****** Inicio do programa ***************
inicio:
	clrf	PORTB		; Inicializa o Port B com zero
	clrf	PORTD		; Inicializa o Port D com zero
	
	banksel	TRISA		; Seleciona banco de mem�ria 1
	
	movlw	0x03
	movwf	TRISB		; Configura PortB como sa�da
	
	movlw	0xc0
	movwf	TRISD		; Configura PortD como sa�da	
	
	movlw	0x00
	movwf	OPTION_REG	; Configura Op��es:
				; Pull-Up habilitados.
				; Interrup��o na borda de subida do sinal no pino B0.
				; Timer0 incrementado pelo oscilador interno.
				; Prescaler WDT 1:1.
				; Prescaler Timer0 1:2.
	banksel PORTA		; Seleciona banco de mem�ria 0.

;******** Loop principal ***************
stand: 
    btfsc PORTB,1	; se apertar M, come�ar o processo
    goto ativar		; come�a
    bcf  PORTB,2	; Desliga led (sensor da Esquerda)

ativar:
    bcf  PORTB,2	; Apaga (sensor da esquerda)
    bsf  PORTB,3	; Liga led (sensor da Direita)
    call delay_ms	; Rotina gasta tempo, carro vai at� o sensor da Direita
    bcf	PORTB,3		; Apaga (sensor da direita)
    bsf  PORTE,5	; Liga Led (Sensor da comporta)
    btfss PORTE,7 	; um if de se apertar outra chave, seguir o c�digo
    call delay_ms	; Rotina 2,  Esperar at� o carrinho encher
    bcf	PORTE,5		; Apaga (comporta)
    call delay_ms	; Rotina 3, carro volta para o ponto inicial
    goto stand		; Volta para o in�cio da execu��o. Deixa lendo as entradas 
   
    end