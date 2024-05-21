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

#define BUFSIZE 5
#define NUMSAMPLES 5

int clear_buffer[BUFSIZE];
int red_buffer[BUFSIZE];
int green_buffer[BUFSIZE];
int blue_buffer[BUFSIZE];
int buffer_index = 0;

unsigned long int clear_avg, red_avg, green_avg, blue_avg;

void initBuffer(){
    /*
     * Function Desc.: Initializes the clear, red, blue, and green buffers for the moving average by setting them all to 0. 
     * Arguments: N/A
     * Peripherals: N/A
    */
    int i;
    for(i = 0; i < BUFSIZE; i++){
        clear_buffer[BUFSIZE] = 0;
        red_buffer[BUFSIZE] = 0;
        green_buffer[BUFSIZE] = 0;
        blue_buffer[BUFSIZE] = 0;
    }
}

void putClear(int clear){
    /*
     * Function Desc.: Puts the 16 bit clear value into the clear value circular buffer
     * Arguments: 16 bit clear value taken from sensor
     * Peripherals: N/A
    */
    clear_buffer[buffer_index++] = clear;
    if(buffer_index >= BUFSIZE){
        buffer_index = 0;
    }
}

void putRed(int red){
    /*
     * Function Desc.: Puts the 16 bit red value into the red value circular buffer
     * Arguments: 16 bit red value taken from sensor
     * Peripherals: N/A
    */
    red_buffer[buffer_index++] = red;
    if(buffer_index >= BUFSIZE){
        buffer_index = 0;
    }
}

void putGreen(int green){
    /*
     * Function Desc.: Puts the 16 bit green value into the green value circular buffer
     * Arguments: 16 bit green value taken from sensor
     * Peripherals: N/A
    */
    green_buffer[buffer_index++] = green;
    if(buffer_index >= BUFSIZE){
        buffer_index = 0;
    }
}

void putBlue(int blue){
    /*
     * Function Desc.: Puts the 16 bit blue value into the blue value circular buffer 
     * Arguments: 16 bit blue value taken from sensor
     * Peripherals: N/A
    */
    blue_buffer[buffer_index++] = blue;
    if(buffer_index >= BUFSIZE){
        buffer_index = 0;
    }
}

unsigned int clearAvg(){
    /*
     * Function Desc.: Loops through the entire clear value circular buffer and averages all the values
     * Arguments: N/A
     * Peripherals: N/A
    */
    int i;
    unsigned long int clear_avg = 0;
    for(i = 0; i < NUMSAMPLES; i++){
        clear_avg += clear_buffer[i];
    }
    clear_avg = clear_avg/NUMSAMPLES;
    return clear_avg;
}

unsigned int redAvg(){
    /*
     * Function Desc.: Loops through the entire red value circular buffer and averages all the values
     * Arguments: N/A
     * Peripherals: N/A
    */
    int i;
    unsigned long int red_avg = 0;
    for(i = 0; i < NUMSAMPLES; i++){
        red_avg += red_buffer[i];
    }
    red_avg = red_avg/NUMSAMPLES;
    return red_avg;
}

unsigned int greenAvg(){
    /*
     * Function Desc.: Loops through the entire green value circular buffer and averages all the values
     * Arguments: N/A
     * Peripherals: N/A
    */
    int i;
    unsigned long int green_avg = 0;
    for(i = 0; i < NUMSAMPLES; i++){
        green_avg += green_buffer[i];
    }
    green_avg = green_avg/NUMSAMPLES;
    return green_avg;
}

unsigned int blueAvg(){
    /*
     * Function Desc.: Loops through the entire blue value circular buffer and averages all the values
     * Arguments: N/A
     * Peripherals: N/A
    */
    int i;
    unsigned long int blue_avg = 0;
    for(i = 0; i < NUMSAMPLES; i++){
        blue_avg += blue_buffer[i];
    }
    blue_avg = blue_avg/NUMSAMPLES;
    return blue_avg;
}
     
void get_color_avg(void){
    /*
     * Function Desc.: Reads all colors , turns them into 16-bit numbers, and puts them into the buffer.
     *                 Does this 5 times before calling the color average functions for average of each color.
     * Arguments: N/A
     * Peripherals: TCS34725 Color Sensor From Adafruit
    */
    int x = 5;
    while(x-- >= 0){
        unsigned int clear = 0, red = 0, green = 0, blue = 0;
             
        //16 bit clear value
        read_color_clear();     //read color
        clear += data_clear[1]; //16 bits number
        clear = (clear << 8) + data_clear[0];
        putClear(clear);
        delay_ms(100);
        
        //16 bit red value
        read_color_red();
        red += data_red[1];
        red = (red << 8) + data_red[0];
        putRed(red);
        delay_ms(100);

        //16 bit green value
        read_color_green();
        green += data_green[1];
        green = (green << 8) + data_green[0];
        putGreen(green);
        delay_ms(100);
        
        //16 bit blue value
        read_color_blue();
        blue += data_blue[1];
        blue = (blue << 8) + data_blue[0];
        putBlue(blue);
        delay_ms(100);
    }
    //average of all colors
    clear_avg = clearAvg();
    red_avg = redAvg();
    green_avg = greenAvg();
    blue_avg = blueAvg();
} 

char identify_color(void){ 
    /*
     * Function Desc.: Takes in all average color values, divides the red with green and red with blue to get R/G and R/B ratios.
     *                 Uses R/G and R/B ratios to accurately identify the paint chips.
     * Arguments: N/A
     * Peripherals: N/A
    */
    float R_G = 0; 
    float R_B = 0;
    
    R_G = ((float)red_avg)/((float)green_avg);
    R_B = ((float)red_avg)/((float)blue_avg);
    
    if( ((R_G >= 3.232)&&(R_G <= 4.3)) && ((R_B >= 3.8)&&(R_B <= 5)) ){
        return 0;   //RED
    }
    if( ((R_G >= 0.562)&&(R_G <= 0.687)) && ((R_B >= 0.390)&&(R_B <= 0.476)) ){
        return 1;   //BLUE
    }
    if( ((R_G >= 0.530)&&(R_G <= 0.647)) && ((R_B >= 1.025)&&(R_B <= 1.253)) ){
        return 2;   //GREEN
    }
    if( ((R_G >= 1.044)&&(R_G <= 1.5)) && ((R_B >= 1.264)&&(R_B <= 1.7)) ){
        return 3;   //BLACK
    }
    if( ((R_G >= 1.389)&&(R_G <= 1.698)) && ((R_B >= 1.658)&&(R_B <= 2.026)) ){
        return 4;   //PINK
    }
    if( ((R_G >= 1.687)&&(R_G <= 2.061)) && ((R_B >= 1.275)&&(R_B <= 1.558)) ){
        return 5;   //MAGENTA
    }
    if( ((R_G >= 1.135)&&(R_G <= 1.387)) && ((R_B >= 1.538)&&(R_B <= 1.880)) ){
        return 6;   //BROWN
    }
    if( ((R_G >= 1.374)&&(R_G <= 1.679)) && ((R_B >= 3.293)&&(R_B <= 4.025)) ){
        return 7;   //YELLOW
    }
    return 10;
}