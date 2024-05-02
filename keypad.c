/*
 * keypad.c
 *
 *  Created on: Feb 7, 2024
 *      Author: Matthew McBurney
 *      Revision: 4/21/2024
 */

#include <msp430.h>
#include "keypad.h"

#define ROW1 BIT0
#define ROW2 BIT1
#define ROW3 BIT2
#define ROW4 BIT3

#define COL1 BIT0
#define COL2 BIT1
#define COL3 BIT2
#define COL4 BIT4

const char colMask = COL1+COL2+COL3+COL4;
const char rowMask = ROW1+ROW2+ROW3+ROW4;




// Function Declarations
char readCols(){
    P2DIR &= ~colMask;                  //Columns Input on port 2
        P2OUT &= ~colMask;                  //Choose Pull-down resistors
        P2REN |= colMask;                   //Enable pull-down resistors

        P5REN &= ~rowMask;                  //Disable resistors
        P5DIR |= rowMask;                   //Rows Output on port 5
        P5OUT |= rowMask;                   //Rows Output 1


        int in = P2IN & colMask;
        switch (in){
        case COL1: return 0;
                   break;
        case COL2: return 1;
                   break;
        case COL3: return 2;
                   break;
        case COL4: return 3;
                   break;
        default:   return 0xFF;
                   break;
    }
}

char readRows(){
    P5DIR &= ~rowMask;                  //Rows Input on port 5
        P5OUT &= ~rowMask;                  //Choose Pull-down resistors
        P5REN |= rowMask;                   //Enable pull-down resistors

        P2REN &= ~colMask;                  //Disable resistors
        P2DIR |= colMask;                   //Columns Output on port 2
        P2OUT |= colMask;                   //Columns Output 1


        int in = P5IN & rowMask;
        switch (in){
            case ROW1: return 0;
                       break;
            case ROW2: return 1;
                       break;
            case ROW3: return 2;
                       break;
            case ROW4: return 3;
                       break;
            default:   return 0xFF;
                       break;
    }
}

char readKeypad(){
    char col = readCols();
    char row = readRows();
    readCols();

    if ((row>=4)||(col>=4)){
            return 0xFF;
    }
    else
            return (col+(row<<2));
}

void keypadInit(){
    //Initialize Keypad Pins
    /*
         * Configure Rows to Output HIGH
         */
        P5DIR |= rowMask;                   //Rows Output on port 5
        P5OUT |= rowMask;                   //Rows Output 1 on Start-up
        P5SEL0 &= ~rowMask;                 //Designate Rows to be GPIO
        P5SEL1 &= ~rowMask;
        P5REN &= ~rowMask;                  //Disable resistors

        /*
         * Configure Columns to Input
         */
        P2DIR &= ~colMask;                  //Columns Input on port 2
        P2OUT &= ~colMask;                  //Pull-down Resister on all columns
        P2SEL0 &= ~colMask;                 //Designate Columns to be GPIO
        P2SEL1 &= ~colMask;
        P2REN |= colMask;                   //Enable pull-down resistors

        /*
         * Enable interupts on port 2 for the columns
         */
        P2IE |= colMask;               // P2.1 interrupt enabled
        P2IES &= ~colMask;

}


