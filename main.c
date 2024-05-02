#include <msp430.h> 
#include "keypad.h"
#include "rgbled.h"


/**
 * Safe that takes in either Keypad entry or combination lock connected to a potentiometer
 * for unlocking. There is a temperature sensor for monitoring the inside of the safe to ensure
 * temperature does not surpass a set limit. Utilizing wifi and a buzzer it notifies the user when the
 * temperature reaches the threshold. The safe will be powered through an external 12V power supply.
 *
 * Authors: Matthew McBurney & Joshua Gross
 *
 * P4.7: Solenoid
 * P4.4: Indoor Light/Door Switch
 * P4.0: Buzzer
 * P6.0: R LED
 * P6.1: G LED
 * P6.2: B LED
 * P1.2: I2C SDA
 * P1.3: I2C SCL
 * P2.0: Col1
 * P2.1: Col2
 * P2.2: Col3
 * P2.4: Col4
 * P5.0: Row1
 * P5.1: Row2
 * P5.2: Row3
 * P5.3: Row4
 * P4.6: Servo
 * P1.1: Potentiometer
 *
 */
enum State{
    IDLE,
    UNLOCKED,
    LOCKOUT
};

enum State state = IDLE;



#define PASSCODELENGTH 4
char passcode[PASSCODELENGTH];
int passPoint = 0;
char failPass = 0;
char lockoutTimer = 0;
char keyVal = 0xFF;
char code[PASSCODELENGTH] = {0,1,2,4};
char keyPress = 0;

/*
 * Initialize necessary GPIO Pins
 */
void gpioInit(){
    //Initialize Solenoid Pin as Output and set initial value
    P4DIR |= BIT7;
    P4OUT &= ~BIT7;

    //Initialize Buzzer Pin as Output
    P4DIR |= BIT0;
    P4OUT &= ~BIT0;

    //Initialize indoor light/switch pin as Input
    P4DIR &= ~BIT4;

    //Initialize KeyPad
    keypadInit();

    //Initialize RGB Led (Uses TB3)
    rgbInit();


    PM5CTL0 &= ~LOCKLPM5;               //Disable the GPIO power-on default high-impedance mode
                                        //to activate previously configured port settings
}

/*
 * Initialize Watchdog Timer
 */
void wdtInit(){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
}

/*
 * Initialize Timers
 */
void timerInit(){

}

/*
 * Open Solenoid aka Unlock Safe
 */
void openSolenoid(){
    P4OUT |= BIT7;
}

/*
 * Close Solenoid aka Lock Safe
 */
void closeSolenoid(){
    P4OUT &= ~BIT7;
}

int checkPasscode() {
    int i;
    for (i = 0; i < PASSCODELENGTH; i++) {
        if (code[i] != passcode[i]) {
            return 0; // Return false if any element doesn't match
        }
    }
    return 1; // Return true if all elements match
}

int main(void)
{
	gpioInit();
	wdtInit();
	timerInit();
	
	__bis_SR_register(GIE);

	while(1){
	    switch (state){
	    case IDLE:
	        if(keyPress){

	            setRGBLED(0,0,255);

	            keyVal = readKeypad();

	            if (keyVal != 0xFF){
	                passcode[passPoint] = keyVal;
	                passPoint ++;
	            }
	            if (passPoint >= PASSCODELENGTH){
	                if (checkPasscode()){
	                    state = UNLOCKED;
	                    failPass = 0;
	                }
	                else {
	                    failPass ++;
	                    if (failPass >= 5){
	                        state = LOCKOUT;
	                    }
	                    else{
	                    setRGBLED(255, 0, 0);
	                    __delay_cycles(1000000);
	                    setRGBLED(0,0,0);
	                    }
	                }
	                passPoint = 0;
	            }

	            __delay_cycles(500000);
	            setRGBLED(0,0,0);
	            keyPress = 0;
	        }

	    break;

	    case UNLOCKED:
	        setRGBLED(0,255,0);
	        openSolenoid();
	        __delay_cycles(5000000);
	        setRGBLED(0,0,0);
	        state = IDLE;
	        closeSolenoid();

	        break;
	    case LOCKOUT:{
	        int i;
	        for (i = 0; i < 5; i++) {
	        setRGBLED(255,0,0);
            __delay_cycles(500000);
            setRGBLED(0,0,0);
            __delay_cycles(500000);
	        }

            state = IDLE;
	        break;
	    }

	    default: break;
	    }




	}


}

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    if (P2IFG & (BIT0+BIT1+BIT2+BIT4))
    {
        if (state == IDLE){
            keyPress = 1;

        }
        P2IFG &= ~(BIT0+BIT1+BIT2+BIT4);
    }
    else P2IFG = 0;
}
