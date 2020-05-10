/*
 * File:   main.c
 * Author: Anel Mandal, Lejla Mehmedagi?
 *
 * Created on January, 2020
 */

#include <xc.h>
#include <stdio.h>

#pragma config FOSC = HS, WDTE = OFF, PWRTE = OFF, MCLRE = ON, CP = OFF, CPD = OFF, BOREN = OFF, CLKOUTEN = OFF
#pragma config IESO = OFF, FCMEN = OFF, WRT = OFF, VCAPEN = OFF, PLLEN = OFF, STVREN = OFF, LVP = OFF

#define _XTAL_FREQ 8000000

#define echo RB2    // Echo pin of HC-SR04
#define trigger RB3 // Trigger pin of HC-SR04
#define LED_RED RB4
#define LED_GREEN RB5
#define buzzer RB6 // I/O pin of YL-44 active buzzer module

int old_centimeters = 0; // To avoid printing unnecessary data
int delay = -1, counter = 0;
__bit measureAgain = 0;

void initialize_ports(){
    ANSELB = 0;
    TRISB = 0b10000111;    
    TRISCbits.TRISC6 = 0;

    buzzer = 1;
    LED_RED = 0;
    LED_GREEN = 1;
}

void initialize_euart(){
    // Baud rate = 9600bps
    BRG16 = 0;
    BRGH = 1;
    SPBRG = 51;
    SYNC = 0;
    SPEN = 1;
    RCIE = 0;
    TXEN = 1;
}

void initialize_interrupts(){
    // IOC on RB2
    IOCBP2 = 1; // Detect a rising edge
    IOCBN2 = 1; // Detect a falling edge
    IOCBF2 = 0; // Clear status flags
    IOCIF = 0;  // Reset IOC flag
    IOCIE = 1;  // Enable Interrupt-On-Change

    // Timer0 for LED and Active Buzzer
    // Timer0 Clock Source Select bit (0 - Internal)
    OPTION_REGbits.TMR0CS = 0;
    // Timer0 Source Edge Select bit (0 - Rising edge)
    OPTION_REGbits.TMR0SE = 0;
    // Prescaler Assignment bit (0 - Prescaler is assigned)
    OPTION_REGbits.PSA = 0;
    // Prescaler Rate Select bits - 1:256 Prescale value
    OPTION_REGbits.PS = 7;

    TMR0IE = 1; // Enable Timer0 interrupt
    TMR0IF = 0; // Reset Timer0 flag

    // Timer1
    T1CONbits.TMR1CS = 0; // Timer1 Clock Source is instruction clock (Fosc/4)
    T1CONbits.T1CKPS = 1; // 1:2 Prescale Value
    TMR1ON = 0;           // Do not start Timer1 yet
    TMR1IE = 1;           // Enable Timer1 interrupt
    TMR1IF = 0;           // Reset Timer1 flag
    TMR1 = 0;             // Clear TMR1 Register

    PEIE = 1; // Set the peripheral interrupt enable bit
    GIE = 1;  // Set the global interrupt enable bit
}

void euart_write(char *text){
    GIE = 0; // Pause all interrupts
    char i;
    for (i = 0; text[i] != '\0'; i++){
        TXREG = text[i];
        while (!TXIF); // Polling TSR register
    }
    GIE = 1; // Enable interrupts again
}

int display_distance(float newCentimeters) {
    trigger = 0;

    if (newCentimeters > 2 && (int)newCentimeters != old_centimeters) {
        char str[10];
        sprintf(str, ",%d", (int)newCentimeters);
        euart_write(str);
    }

    if (newCentimeters > 2 && newCentimeters <= 50) {
        LED_GREEN = 0;
        delay = (char)(newCentimeters / 50.0 * 8 + 1);
    }
    else {
        LED_RED = 0;
        LED_GREEN = 1;
        buzzer = 1;
        delay = -1;
    }

    return newCentimeters;
}

void measure_distance(void){
    // Initiate distance measuring again
    TMR1 = 0;
    TMR1ON = 1;
    trigger = 1;
    while (TMR1 <= 10); // 10 microseconds impulse on Trigger pin
    trigger = 0;
    TMR1ON = 0;
}

void __interrupt() handle_interrupt(void){

    if (TMR0IE && TMR0IF){
        TMR0IF = 0;
        counter++;
        if (counter >= delay && delay != -1){
            counter = 0;
            LED_RED = 1 - LED_RED;
            buzzer = 1 - buzzer;
        }
    }

    if (IOCIE && IOCIF && IOCBF2){
        IOCIF = 0;
        IOCBF2 = 0;

        if (echo){
            // Start TIMER1
            TMR1 = 0;
            TMR1ON = 1;
        }
        else {
            // Stop TIMER1
            TMR1ON = 0;
            old_centimeters = display_distance(0.01715 * TMR1);
            measureAgain = 1;
        }
    }
}

void main(void){
    initialize_ports();
    initialize_euart();
    initialize_interrupts();

    while (1){
        if (measureAgain){
            measureAgain = 0;
            measure_distance();
        }
    }
}

