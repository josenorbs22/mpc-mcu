/*
 * File:   main.c
 * Autor: José Norberto Fagundes Isaias (Mtrcl. 19202785)
 * Universidade Federal de Santa Catarina
 * Microprocessadores e Microcontroladores
 * Controle de cancela num cruzamento de trilho de trem com via de carros
 *
 * Created on 28 de Junho de 2022, 15:36
 */


#include <xc.h>
#include <pic16f877a.h>

#define _XTAL_FREQ 4000000

#define ST PORTBbits.RB0   //Sensor de presença do trem
#define SC PORTBbits.RB1   //Sensor de presença do carro
#define SF PORTBbits.RB2   //Sensor de cancela fechada
#define SA PORTBbits.RB3   //Sensor de cancela aberta
#define CA PORTBbits.RB4   //Acionador do relay para abrir a cancela
#define CF PORTBbits.RB5   //Acionador do relay para fechar a cancela
#define LF PORTBbits.RB6   //Luz para indicar que a cancela está fechando
#define LA PORTBbits.RB7   //Luz para indicar que a cancela está abrindo

__bit flag, carroPresenca, tremPresenca, cancela;


void __interrupt() TrataInterrupcao(void){
    //Tratamento da interrupção que acontece quando o trem passa pelo sensor no trilho
    if(INTF){
        flag = 1;
        INTCONbits.INTF = 0;
        //Fecha a cancela e liga ambas as luzes intermitentemente até o trem sair do sensor
        while(ST == 0){
            /*Aciona o motor para fechar e acende a luz indicadora até a cancela chegar no sensor de cancela fechada*/
            while(SF == 1){
                CF = 0;
                LF = 1;
                LA = 0;
                __delay_ms(150);
                LF = 0;
                LA = 1;
                __delay_ms(150);
            }
            //Desliga o motor após fechar a cancela e desliga a luz indicadora
            CF = 1;
            LF = 1;
            LA = 0;
            __delay_ms(150);
            LF = 0;
            LA = 1;
            __delay_ms(150);
        }
        __delay_ms(10000);
        LF = 1;
        LA = 1;
        INTCONbits.RBIF = 0;
    }
    return;
}
void main(void) {
    TRISB = 0b00001111;
    OPTION_REG = 0b00111111;
    INTCON = 0b10010000;
    
    LF = 1;
    LA = 1;
    CA = 1;
    CF = 1;
    
    while(1){
        flag = 0;
        //Analisa se há algum carro no sensor e faz o processo de abrir a cancela e fechar
        if(SC == 0 && flag == 0){
            //Aciona o motor para abrir a cancela e acende a luz indicadora até a cancela chegar no sensor de cancela aberta
            while(SA == 1){
              CA = 0;
              LA = 0;
            }
            //Desliga o motor após abrir a cancela e desliga a luz indicadora
            CA = 1;
            LA = 1;
            //Espera um tempo até o carro sair
            __delay_ms(5000);
            /*Aciona o motor para fechar a cancela se não tiver nenhum carro no sensor
             *  e acende a luz indicadora até a cancela chegar no sensor de cancela fechada*/
            while(SF == 1){
              CF = 0;
              LF = 0;
            }
            //Desliga o motor após fechar a cancela e desliga a luz indicadora
            CF = 1;
            LF = 1;
        }
    }
    
    return;
}