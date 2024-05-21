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

void lcd_cmd(char Package) {
    I2C2CONbits.SEN = 1;    
    //SEN = 1; //Initiate Start condition
    while(I2C2CONbits.SEN == 1);
    //Wait for SEN == 0  // SEN will clear when Start Bit is complete
    IFS3bits.MI2C2IF = 0;
    //Clear IFS3bits.MI2C2IF
    I2C2TRN = 0b01111100; // 8-bits consisting of the slave address and the R/nW bit
    while(IFS3bits.MI2C2IF == 0);
    //Wait for IFS3bits.MI2C2IF == 1 // *Refer to NOTE below*
    IFS3bits.MI2C2IF = 0;
    //Clear IFS3bits.MI2C2IF;
    I2C2TRN = 0b00000000; // 8-bits consisting of control byte
    while(IFS3bits.MI2C2IF == 0);
    //Wait for IFS3bits.MI2C2IF == 1
    IFS3bits.MI2C2IF = 0;
    //Clear IFS3bits.MI2C2IF
    I2C2TRN = Package; // 8-bits consisting of the data byte
    while(IFS3bits.MI2C2IF == 0);
    //Wait for IFS3bits.MI2C2IF == 1
    IFS3bits.MI2C2IF = 0;
    //Clear IFS3bits.MI2C2IF
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1);
    //Wait for PEN==0 // PEN will clear when Stop bit is complete
}

void lcd_init(){    //looked like it should have a little more
    I2C2BRG = 157;
    IFS3bits.MI2C2IF = 0;
    I2C2CONbits.I2CEN = 1;
   
    delay_ms(40);
    
    lcd_cmd(0b00111000); // function set
    lcd_cmd(0b00111001); // function set, advance instruction mode
    lcd_cmd(0b00010100); // interval osc
    lcd_cmd(0b01110000); // contrast Low
    lcd_cmd(0b01010110);
    lcd_cmd(0b01101100); // follower control
    
    delay_ms (200);
    
    lcd_cmd(0b00111000); // function set 
    lcd_cmd(0b00001100); // Display On 
    lcd_cmd(0b00000001); // Clear Display
    
    delay_ms(1);
}

void lcd_printChar(char Package) {
    I2C2CONbits.SEN = 1; 
    //SEN = 1; //Initiate Start condition
    while(I2C2CONbits.SEN == 1);
    //Wait for SEN == 0  // SEN will clear when Start Bit is complete
    IFS3bits.MI2C2IF = 0;
    //Clear IFS3bits.MI2C2IF
    I2C2TRN = 0b01111100; // 8-bits consisting of the slave address and the R/nW bit
    while(IFS3bits.MI2C2IF == 0);
    //Wait for IFS3bits.MI2C2IF == 1
    IFS3bits.MI2C2IF = 0;
    //Clear IFS3bits.MI2C2IF
    I2C2TRN = 0b01000000; // 8-bits consisting of control byte /w RS=1
    while(IFS3bits.MI2C2IF == 0);
    //Wait for IFS3bits.MI2C2IF == 1
    IFS3bits.MI2C2IF = 0;
    //Clear IFS3bits.MI2C2IF
    I2C2TRN = Package; // 8-bits consisting of the data byte
    while(IFS3bits.MI2C2IF == 0);
    //Wait for IFS3bits.MI2C2IF == 1
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1); // PEN will clear when Stop bit is complete
    //Wait for PEN==0 // PEN will clear when Stop bit is complete
}

void lcd_setCursor(char x, char y){
    char addressB;
    addressB = 0x40 * x  +  y;
    addressB = addressB + 0b0010000000;
    lcd_cmd(addressB);
}

void lcd_printStr(const char s[]){
    int s_len = strlen(s);
    int i;
    
    I2C2CONbits.SEN = 1; 
    //SEN = 1; //Initiate Start condition
    while(I2C2CONbits.SEN == 1);
    //Wait for SEN == 0  // SEN will clear when Start Bit is complete
    IFS3bits.MI2C2IF = 0;
    //Clear IFS3bits.MI2C2IF
    I2C2TRN = 0b01111100; // 8-bits consisting of the slave address and the R/nW bit
    while(IFS3bits.MI2C2IF == 0);
    //Wait for IFS3bits.MI2C2IF == 1
    IFS3bits.MI2C2IF = 0;
    //Clear IFS3bits.MI2C2IF
    
    //need to account for the address of the bits
    for(i = 0; i < s_len; i++){   //iterates through every element of string till the null element
        if(i == s_len - 1){             //checks if it is the last char that needs to be sent out and makes the control byte 0
            I2C2TRN = 0b01000000; // 8-bits consisting of control byte = 0 /w RS=1
            while(IFS3bits.MI2C2IF == 0);
            //Wait for IFS3bits.MI2C2IF == 1
            IFS3bits.MI2C2IF = 0;
            //Clear IFS3bits.MI2C2IF
            I2C2TRN = s[i]; // 8-bits consisting of the data byte
            while(IFS3bits.MI2C2IF == 0);
            //Wait for IFS3bits.MI2C2IF == 1
            IFS3bits.MI2C2IF = 0;
            //Clear IFS3bits.MI2C2IF
        }else{            
            I2C2TRN = 0b11000000; // 8-bits consisting of control byte /w RS=1
            while(IFS3bits.MI2C2IF == 0);
            //Wait for IFS3bits.MI2C2IF == 1
            IFS3bits.MI2C2IF = 0;
            //Clear IFS3bits.MI2C2IF
            I2C2TRN = s[i]; // 8-bits consisting of the data byte
            while(IFS3bits.MI2C2IF == 0);
            //Wait for IFS3bits.MI2C2IF == 1
            IFS3bits.MI2C2IF = 0;
            //Clear IFS3bits.MI2C2IF
        }
    }
    
    I2C2CONbits.PEN = 1;
    while(I2C2CONbits.PEN == 1); // PEN will clear when Stop bit is complete
    //Wait for PEN==0 // PEN will clear when Stop bit is complete    
}

void clearLCD(){
    lcd_setCursor(0,0);
    lcd_printStr("            "); //8 clear spaces
    lcd_setCursor(1,0);
    lcd_printStr("            "); //8 clear spaces
}
