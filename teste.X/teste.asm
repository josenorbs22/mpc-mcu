;*******************************************************************************
; UFSC- Universidade Federal de Santa Catarina
; Projeto: Prensa pneumática controlada por botões
; Autor: José Norberto
; Prensa pneumática
;*******************************************************************************
    
#include<p12f675.inc>

; Palavra de configuracao, desabilita e habilita algumas op??es internas
; CONFIG
; __config 0x31D7
 __CONFIG _FOSC_EXTRCCLK & _WDTE_OFF & _PWRTE_OFF & _MCLRE_OFF & _BOREN_ON & _CP_OFF & _CPD_OFF ;Clock Externo
  ; __CONFIG _FOSC_INTRCCLK & _WDTE_OFF & _PWRTE_OFF & _MCLRE_OFF & _BOREN_ON & _CP_OFF & _CPD_OFF ;Clock interno
    
  cblock 0x20
    tempo0
    tempo1
    tempo2
    filtro
  endc
  
;======= Vetor de início==============
  org 0x00
  goto inicio

;======= Rotinas e sub-rotinas =======
;Rotina de acionamento da prensa(LED)
acionar:
    bcf	    GPIO,1
    bsf	    GPIO,2
    call    delay_ms
    bcf	    GPIO,2
    bsf	    GPIO,1
;Rotina de delay
delay_ms:
    movlw   .8
    movwf   tempo2
denovo2:
    movlw   .2000
    movwf   tempo1
denovo:
    movlw   .250
    movwf   tempo0
volta:
    nop
    decfsz  tempo0, F
    goto    volta
    decfsz  tempo1, F
    goto    denovo
    decfsz  tempo2, F
    goto    denovo2
return


;======Programa=======
inicio:
    banksel GPIO
    bcf	    STATUS,RP0  ;Bank 0
    clrf    GPIO	;Init GPIO
    movlw   0x0f	;Set GP<3:0> to 
    movwf   CMCON	;digital IO
    bsf	    STATUS,RP0  ;Bank 1
    clrf    ANSEL	;Digital I/O
    movlw   0x01	;Set GP<3:0> as inputs
    movwf   TRISIO	;and set GP<2:1> as output
    
    banksel TRISIO
    
    movlw   0x00
    movwf   OPTION_REG
    
    movlw   0x00
    movwf   INTCON

    movlw   0x00
    movwf   ADCON0
    
    banksel GPIO
    bsf	    GPIO,1	;recuar pistao
    call    delay_ms
    
;=====Loop do programa=====
loop:
    btfsc   GPIO,0
    goto    loop
    btfsc   GPIO,3
    goto    loop
    call    acionar
    goto    loop
    
end