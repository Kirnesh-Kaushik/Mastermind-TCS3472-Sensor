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

void delay_ms(unsigned int ms){
    /*
     * Function Desc.: Calls a delay of some milliseconds
     * Arguments: Variable value corresponds to how many ms you want. (i.e 50 is equal to 50 ms) 
     * Peripherals: N/A
    */
    while(ms-- > 0){
    asm("repeat #15998");
    asm("nop");
    }
}