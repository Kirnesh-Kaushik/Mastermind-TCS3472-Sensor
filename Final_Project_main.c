/*
Date: April 26, 2024
Names: Kirnesh Kaushik, Sam OConnor, Emily Schaefer
Course number: EE 2361
Term: Spring 2024
*/

#include "time.h"
#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <math.h>
#include "xc.h"

// Configuration Words
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is enabled)
#pragma config FNOSC = FRCPLL      // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

//Header Files
#include "Final_Project_Color_Sensor.h"
#include "Final_Project_Color_Moving_Average.h"
#include "Final_Project_LCD.h"
#include "Final_Project_Button.h"
#include "Final_Project_Game.h"
#include "Final_Project_Delay.h"

void pic24_init(){
    _RCDIV = 0;             //set frequence to 16 MHz
    AD1PCFG = 0xffff;       //set all pins to digital
}

int main(void) {
    /*
     * Initalizes the pic, lcd, the button, and color sensor. Generates a random color code and prepares the game for 3 tries. 
     * Reads the device id as well. 
     * No return value because of main function. 
    */
      
    srand(time(NULL)); //initializing seed for random function
    pic24_init();
    lcd_init();
    color_init();
    initializeButton();
    TRISBbits.TRISB6 = 0; // Set the LED control pin as output
//    masterCodeGen();   //supposed to generate random color code but doesn't right now
    clearLCD();
    
    //get device id
    int device_id_main;
    device_id_main = read_device_id();
    delay_ms(100);
    
    //Try 1
    get_color_inputs();     //gets 4 color inputs from 4 button presses and puts them into an array
    calculateCompare();     //calculates which color codes are in the right spot
    printCompare(0);        //print to LCD the progress on row 1
    winCondition();
    
    //Try 2
    get_color_inputs();     //gets 4 color inputs from 4 button presses and puts them into an array
    calculateCompare();     //calculates which color codes are in the right spot
    printCompare(1);         //print to LCD the progress on row 2
    winCondition();
    
    //Try 3
    get_color_inputs();     //gets 4 color inputs from 4 button presses and puts them into an array
    calculateCompare();     //calculates which color codes are in the right spot
    winCondition();
    
    //If the user has not won after 3 attempts, the answer is given and the user has lost
    while(1){
        clearLCD();
        lcd_setCursor(0,0);
        lcd_printStr("Try");
        lcd_setCursor(1,0);
        lcd_printStr("Again");
    }
    return 0;
}