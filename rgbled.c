/*
 * rgbled.c
 *
 * Author: Matthew McBurney
 * Date: 4/21/2024
 */
#include <msp430.h>

const char ledMask = BIT0+BIT1+BIT2;

void rgbInit()
{
    // Configure pin 6.0,6.1,6.2 for hardware PWM output
    P6DIR |= ledMask;
    P6SEL0 |= ledMask;
    P6SEL1 &= ~ledMask;

    // Configure Timer B3.1 for PWM
    TB3CCR0 = 255;    // PWM Period
    TB3CCR1 = 0;     // PWM Duty Cycle
    TB3CCR2 = 0;     // PWM Duty Cycle
    TB3CCR3 = 0;     // PWM Duty Cycle
    TB3CCTL1 = OUTMOD_3; // Set/reset output mode
    TB3CCTL2 = OUTMOD_3; // Set/reset output mode
    TB3CCTL3 = OUTMOD_3; // Set/reset output mode

    // Configure Timer B3
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR; // SMCLK, Up mode, Clear TBR


}



void setRGBLED(char red, char blu, char grn)
{
    TB3CCR1 = red;
    TB3CCR2 = blu;
    TB3CCR3 = grn;

}
