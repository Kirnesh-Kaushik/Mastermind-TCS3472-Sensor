#include "time.h"
#include <stdlib.h>
#include <stdio.h> 
#include <string.h> 
#include <math.h>
#include "xc.h"

//Header Files
#include "Final_Project_Color_Sensor.h"
#include "Final_Project_Color_Moving_Average.h"
#include "Final_Project_LCD.h"
#include "Final_Project_Button.h"
#include "Final_Project_Game.h"
#include "Final_Project_Delay.h"

void initializeButton(){
    /*
     * Function: initializes the button as an input as well as the internal pull up resistor for pin RB15
     * Arguments: N/A
    */
    TRISBbits.TRISB15 = 1; // Set the button pin as input
    CNPU1bits.CN11PUE = 1; // Enable the internal pull-up resistor
}

void get_color_inputs(void){
    /*
     * Function Desc.: Function polls for a button four times and records the color into an array of size 4 (userCode). 
     *                 Does this by calling the average color function and then getting then calling the identify color function. 
     * Arguments: N/A 
     * Peripherals: Function activates on a button press but it also calls functions that use the color sensor. 
    */
    
    int pressed = 0;
    int pressed2 = 0;
    int count = 0;
    
    while(1){
        int color_code;
        if(count == 4){
            break;
        }
        else{
            pressed2 = PORTBbits.RB15;
            delay_ms(2);
            if(pressed2 == 0 && pressed == 1) {
                //led turns on
                LATB = 0x0040;
                pressed = 1;
                get_color_avg();
                color_code = identify_color();
                if(color_code < 10 && color_code >= 0){
                    userCode[count] = color_code;
                    LATB = 0x0000;
                    count++;
                } else{
                }
           
            }
            if(pressed2 == 1 && pressed == 0) {
                pressed = 0;
            }
            pressed = pressed2;
        }
    }
}
