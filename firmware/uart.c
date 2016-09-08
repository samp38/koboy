#include "pic24_all.h"

void U1TX_callback(void);

void uart2_Config(void) {
    char tmp;
    U2BRG  = 12;     // 8928baud @ FCY = 250kHz, note that you must select "custom baud rate" in the
                   // pickit2 uart application drop-down window for baudrate, and enter the value 8928
    U2MODEbits.BRGH = 1;
    U2MODEbits.UARTEN = 1;
    U2MODEbits.RTSMD = 1; // no flow control
    U2MODEbits.PDSEL = 0; // 8 bits, no parity
    U2MODEbits.STSEL = 0; // 1 stop bit
    while(U2STAbits.URXDA == 1) {
        tmp = U2RXREG;
    }
    // clear IF flags
    IFS1bits.U2RXIF = 0;
    IFS1bits.U2TXIF = 0;
    // set priority
    IPC7bits.U2RXIP = 1;
    IPC7bits.U2TXIP = 2;
    // disable interrupt
    IEC1bits.U2RXIE = 0;
    IEC1bits.U2TXIE = 0;
    // enable transmit
    U2STAbits.UTXEN = 1;
}

void uart1_Config(void) {
    char tmp;
    U1BRG  = 12;     // 9600 baud @ FCY = 250kHz
    U1MODEbits.BRGH = 1;
    U1MODEbits.UARTEN = 1;
    U1MODEbits.RTSMD = 1; // no flow control
    U1MODEbits.PDSEL = 0; // 8 bits, no parity
    U1MODEbits.STSEL = 0; // 1 stop bit
    while(U1STAbits.URXDA == 1) {
        tmp = U1RXREG;
    }
    // clear IF flags
    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF = 0;
    // set priority
    IPC2bits.U1RXIP = 1;
    IPC3bits.U1TXIP = 1;
    // disable interrupt
    IEC0bits.U1RXIE = 0;
    IEC0bits.U1TXIE = 0;
    // enable transmit
    U1STAbits.UTXEN = 0;
    U1STAbits.UTXISEL0 = 1;
    U1STAbits.UTXISEL1 = 0;
}

void __attribute__((__interrupt__, auto_psv)) _U1TXInterrupt(void){
    IFS0bits.U1TXIF = 0;
    U1TX_callback();
 }

void uart_Print(char *szBuf) {
    while(*szBuf) {
        U2TXREG = *szBuf++;   // transfer data byte to TX reg
        while(!U2STAbits.TRMT);  // wait for xmit to complete
    }
}


