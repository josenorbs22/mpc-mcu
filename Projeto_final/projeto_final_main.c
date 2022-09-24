/*
 * File:   projeto_final_main.c
 * Author: José Norberto Fagundes Isaias e Cassiano Matias Luzzietti
 * 
 * Projeto final para a disciplina de Microprocessadores e Microcontroladores
 * 
 * Este sistema serve para automação residencial, controlando a luminosidade do
 * ambiente para economia de energia e conforto do habitante do cômodo
 *
 * Created on 23 de Julho de 2022, 10:27
 */


#include <xc.h>
#include <pic16f877a.h>
#include <stdio.h>
#include "lcd.h"

#define _XTAL_FREQ 4000000

#pragma config WDTE = ON
#pragma config FOSC = HS    
#pragma config PWRTE = ON 
#pragma config BOREN = ON 

/*#define RS RD4
#define EN RD6
#define D4 RD3
#define D5 RD2
#define D6 RD1
#define D7 RD0*/
#define SCortA RB2
#define SCortF RB3
#define SPre RB0
#define SLamp RB6
#define SCadeira RB7
#define Lamp RC2
#define M1 RC3
#define M2 RC4
#define Alarm RC5

__bit flag, ctrl, presenca;
int conta, valor;
unsigned long conta2;

void __interrupt() TrataInterrupcao(void){
    if(TMR1IF){
        PIR1bits.TMR1IF = 0;
        TMR1L = 0b11011100;
        TMR1H = 0b00001011;
        
        conta++;
        
        if(conta == 5400){
            Lcd_Clear();
            Lcd_Set_Cursor(1, 1);
            Lcd_Write_String("Muito tempo sentado.");
            Lcd_Set_Cursor(2, 1);
            Lcd_Write_String("Levante e se exercite!");
            for(int i = 0; i < 8; i++){
                Alarm = 1;
                __delay_ms(1000);
                Alarm = 0;
                Lcd_Shift_Left();
                CLRWDT();
            }
            Lcd_Clear();
            conta = 0;
        }
    }
    
    if (ADIF){  
        PIR1bits.ADIF = 0; 
      
        //valor = ADRESH;        
    }
    
    if(TMR2IF){
        PIR1bits.TMR2IF = 0;
        TMR2 = 0b00000110;
        
        conta2++;

        if(conta2 >= 75000){
            Lcd_Clear();
            Lamp = 0;
            conta2 = 0;
            presenca = 0;
        }
    }
    if(INTF){
        INTCONbits.INTF = 0;
        conta2 = 0;
        TMR2 = 0b00000110;
        presenca = 1;
        Lamp = 1;
        INTCONbits.RBIF = 0;
    }
    return;
}

void ControleCortina(){
    if(ctrl == 1){
        while(SCortF != 0){
            M1 = 0;
            M2 = 1;
        }       
        M1 = 1;
        M2 = 1;
    } else {
        while(SCortA != 0){
            M1 = 1;
            M2 = 0;
        }        
        M1 = 1;
        M2 = 1;
    }
}

void main(void) {
    char valorC[4];
    conta = 0;
    conta2 = 0;
    presenca = 0;
    TRISA = 1;
    TRISB = 0b11111101;
    TRISC = 0x00;
    TRISD = 0x00;
    OPTION_REGbits.nRBPU = 0;
    OPTION_REGbits.INTEDG = 0;
    
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    
    
    INTCONbits.GIE=1;  
    INTCONbits.PEIE = 1;
    INTCONbits.INTE = 1;
    //INTCONbits.RBIE = 1;
    PIE1bits.TMR1IE = 1; 
    PIE1bits.TMR2IE = 1;
    PIE1bits.ADIE = 1;
    
    T1CONbits.TMR1CS = 0; 
    T1CONbits.T1CKPS0 = 1; 
    T1CONbits.T1CKPS1 = 1;
    T2CONbits.T2CKPS1 = 1;
    T2CONbits.TOUTPS0 = 0;
    T2CONbits.TOUTPS1 = 0;
    T2CONbits.TOUTPS2 = 0;
    T2CONbits.TOUTPS3 = 0;
    TMR1L = 0b11011100;
    TMR1H = 0b00001011;
    TMR2 = 0b00000110;
    
    ADCON1bits.PCFG0 = 0;  
    ADCON1bits.PCFG1 = 1;  
    ADCON1bits.PCFG2 = 1;  
    ADCON1bits.PCFG3 = 1;
    
    ADCON0bits.CHS0 = 0;
    ADCON0bits.CHS1 = 0; 
    ADCON0bits.CHS2 = 0;
    
    ADCON0bits.ADCS0 = 0;   
    ADCON0bits.ADCS1 = 0;
    //ADCON1bits.ADCS2 = 0;
    ADCON1bits.ADFM = 0;
    
    ADCON0bits.ADON = 1; 
    T1CONbits.TMR1ON = 1;
    T2CONbits.TMR2ON = 1;
    CLRWDT();
            
    ADRESL = 0x00;
    ADRESH = 0x00;
    
    Lcd_Init();
    Lcd_Clear();
    
    
    while(1){
        CLRWDT();
        //Lcd_Clear();
        if(SCadeira == 1){
            conta = 0;
            TMR1L = 0b11011100;
            TMR1H = 0b00001011;
            T1CONbits.TMR1ON = 0;
        } else if(T1CONbits.TMR1ON == 0){
            T1CONbits.TMR1ON = 1;
        }
        
        //if(SPre0 == 1)
        
        ADCON0bits.GO = 1;
        __delay_us(10);
        if(SLamp == 0 && presenca == 1){
            ctrl = 1;
            Lamp = 1;
            ControleCortina(1);
        } else{
            ctrl = 0;
            Lamp = 0;
            ControleCortina(0);
        }
        valor = ADRESH;
        valor *= 2;
        Lcd_Set_Cursor(2, 1);
        Lcd_Write_String("Temperatura: ");
        sprintf(valorC, "%do", valor);
        
        Lcd_Write_String(valorC);
    }
    return;
}
