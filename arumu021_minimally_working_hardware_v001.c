/*
 * File:   arumu021_minimally_working_hardware_v001.c
 * Author: 
 *
 * Created on April 9, 2024, 2:37 PM
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

#define BUFSIZE 5
#define NUMSAMPLES 5

int clear_buffer[BUFSIZE];
int red_buffer[BUFSIZE];
int green_buffer[BUFSIZE];
int blue_buffer[BUFSIZE];
int buffer_index = 0;

//first element = lower byte
//second element = higher byte
unsigned char data_clear[2] = {0,0};// clear data stored from ADC
unsigned char data_red[2] = {0,0};// red data stored from ADC
unsigned char data_green[2] = {0,0};// green data stored from ADC
unsigned char data_blue[2] = {0,0}; // blue data stored from ADC

unsigned long int clear_avg, red_avg, green_avg, blue_avg;

int codeCompare[4]; //compared code 
int masterCode[4] = {0,2,1,0}; //Random generated code
int userCode[4]; //User input codes
int winCond = 0;
int didYouWin = 0;

void delay_ms(unsigned int ms){
    while(ms-- > 0){
    asm("repeat #15998");
    asm("nop");
    }
}

void pic24_init(){
    _RCDIV = 0;             //set frequence to 16 MHz
    AD1PCFG = 0xffff;       //set all pins to digital
}

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

void read_color_clear(void){
    //SEN = 1; //Initiate Start condition
    I2C1CONbits.SEN = 1;    
    while(I2C1CONbits.SEN == 1);
   
    //slave address with write bit    10010010
    I2C1TRN = 0b01010010; // 8-bits consisting of the slave address (0x29 = 0b0101001) with the write bit (0)
    while(I2C1STATbits.TRSTAT == 1); 
   
    //command byte with register address
    I2C1TRN = 0b10110100; // sending register address = 0x14 with auto increment
    while(I2C1STATbits.TRSTAT == 1);
   
    //repeated start condition
    I2C1CONbits.RSEN = 1;   //repeated start
    while(I2C1CONbits.RSEN == 1);   //repeated start
   
    //slave address with read bit
    I2C1TRN = 0b01010011; // 8-bits consisting of the register address (0x29 = 0b0101001) with the read bit (1)
    while(I2C1STATbits.TRSTAT == 1);
   
    //receive data byte 1
    I2C1CONbits.RCEN = 1;
    while(I2C1STATbits.RBF == 0);
    data_clear[0] = I2C1RCV;
   
    //sending ACK signal from pic    
    I2C1CONbits.ACKDT = 0;              
    I2C1CONbits.ACKEN = 1;              
    while(I2C1CONbits.ACKEN == 1);
    
    //receive data byte 2
    I2C1CONbits.RCEN = 1;
    while(I2C1STATbits.RBF == 0);
    data_clear[1] = I2C1RCV;
   
    //sending NACK signal from pic    
    I2C1CONbits.ACKDT = 1;              
    I2C1CONbits.ACKEN = 1;              
    while(I2C1CONbits.ACKEN == 1);
    
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN == 1);
    //Wait for PEN==0 // PEN will clear when Stop bit is complete
}

void read_color_red(void){ 
    //SEN = 1; //Initiate Start condition
    I2C1CONbits.SEN = 1;    
    while(I2C1CONbits.SEN == 1);
   
    //slave address with write bit    10010010
    I2C1TRN = 0b01010010; // 8-bits consisting of the slave address (0x29 = 0b0101001) with the write bit (0)
    while(I2C1STATbits.TRSTAT == 1); 
   
    //command byte with register address
    I2C1TRN = 0b10110110; // sending register address = 0x16 with auto increment
    while(I2C1STATbits.TRSTAT == 1);
   
    //repeated start condition
    I2C1CONbits.RSEN = 1;   //repeated start
    while(I2C1CONbits.RSEN == 1);   //repeated start
   
    //slave address with read bit
    I2C1TRN = 0b01010011; // 8-bits consisting of the register address (0x29 = 0b0101001) with the read bit (1)
    while(I2C1STATbits.TRSTAT == 1);
   
    //receive data byte 1
    I2C1CONbits.RCEN = 1;
    while(I2C1STATbits.RBF == 0);
    data_red[0] = I2C1RCV;
   
    //sending ACK signal from pic    
    I2C1CONbits.ACKDT = 0;              
    I2C1CONbits.ACKEN = 1;              
    while(I2C1CONbits.ACKEN == 1);
    
    //receive data byte 2
    I2C1CONbits.RCEN = 1;
    while(I2C1STATbits.RBF == 0);
    data_red[1] = I2C1RCV;
   
    //sending NACK signal from pic    
    I2C1CONbits.ACKDT = 1;              
    I2C1CONbits.ACKEN = 1;              
    while(I2C1CONbits.ACKEN == 1);
    
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN == 1);
    //Wait for PEN==0 // PEN will clear when Stop bit is complete
}

void read_color_green(void){
    //SEN = 1; //Initiate Start condition
    I2C1CONbits.SEN = 1;    
    while(I2C1CONbits.SEN == 1);
   
    //slave address with write bit    10010010
    I2C1TRN = 0b01010010; // 8-bits consisting of the slave address (0x29 = 0b0101001) with the write bit (0)
    while(I2C1STATbits.TRSTAT == 1); 
   
    //command byte with register address
    I2C1TRN = 0b10111000; // sending register address = 0x18 with auto increment
    while(I2C1STATbits.TRSTAT == 1);
   
    //repeated start condition
    I2C1CONbits.RSEN = 1;   //repeated start
    while(I2C1CONbits.RSEN == 1);   //repeated start
   
    //slave address with read bit
    I2C1TRN = 0b01010011; // 8-bits consisting of the register address (0x29 = 0b0101001) with the read bit (1)
    while(I2C1STATbits.TRSTAT == 1);
   
    //receive data byte 1
    I2C1CONbits.RCEN = 1;
    while(I2C1STATbits.RBF == 0);
    data_green[0] = I2C1RCV;
   
    //sending ACK signal from pic    
    I2C1CONbits.ACKDT = 0;              
    I2C1CONbits.ACKEN = 1;              
    while(I2C1CONbits.ACKEN == 1);
    
    //receive data byte 2
    I2C1CONbits.RCEN = 1;
    while(I2C1STATbits.RBF == 0);
    data_green[1] = I2C1RCV;
   
    //sending NACK signal from pic    
    I2C1CONbits.ACKDT = 1;              
    I2C1CONbits.ACKEN = 1;              
    while(I2C1CONbits.ACKEN == 1);
    
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN == 1);
    //Wait for PEN==0 // PEN will clear when Stop bit is complete
}

void read_color_blue(void){
    //SEN = 1; //Initiate Start condition
    I2C1CONbits.SEN = 1;    
    while(I2C1CONbits.SEN == 1);
   
    //slave address with write bit    10010010
    I2C1TRN = 0b01010010; // 8-bits consisting of the slave address (0x29 = 0b0101001) with the write bit (0)
    while(I2C1STATbits.TRSTAT == 1); 
   
    //command byte with register address
    I2C1TRN = 0b10111010; // sending register address = 0x1A with auto increment
    while(I2C1STATbits.TRSTAT == 1);
   
    //repeated start condition
    I2C1CONbits.RSEN = 1;   //repeated start
    while(I2C1CONbits.RSEN == 1);   //repeated start
   
    //slave address with read bit
    I2C1TRN = 0b01010011; // 8-bits consisting of the register address (0x29 = 0b0101001) with the read bit (1)
    while(I2C1STATbits.TRSTAT == 1);
   
    //receive data byte 1
    I2C1CONbits.RCEN = 1;
    while(I2C1STATbits.RBF == 0);
    data_blue[0] = I2C1RCV;
   
    //sending ACK signal from pic    
    I2C1CONbits.ACKDT = 0;              
    I2C1CONbits.ACKEN = 1;              
    while(I2C1CONbits.ACKEN == 1);
    
    //receive data byte 2
    I2C1CONbits.RCEN = 1;
    while(I2C1STATbits.RBF == 0);
    data_blue[1] = I2C1RCV;
   
    //sending NACK signal from pic    
    I2C1CONbits.ACKDT = 1;              
    I2C1CONbits.ACKEN = 1;              
    while(I2C1CONbits.ACKEN == 1);
    
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN == 1);
    //Wait for PEN==0 // PEN will clear when Stop bit is complete
}

void i2c_write_byte(char device_address, char register_address, char data) {
    // Initiate Start condition
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN == 1); // Wait for Start Bit to complete
   
    // Send slave device address with write bit (0)
    I2C1TRN = (device_address << 1) | 0b00000000;
    while(I2C1STATbits.TRSTAT == 1); // Wait for transmit to complete
   
    // Send register address
    I2C1TRN = 0b1000000 | register_address;
    while(I2C1STATbits.TRSTAT == 1); // Wait for transmit to complete
   
    // Send data byte
    I2C1TRN = data;
    while(I2C1STATbits.TRSTAT == 1); // Wait for transmit to complete
   
    // Initiate Stop condition
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN == 1); // Wait for Stop Bit to complete
}

char read_device_id(void){
    char device_id;
   
    //SEN = 1; //Initiate Start condition
    I2C1CONbits.SEN = 1;    
    while(I2C1CONbits.SEN == 1);
   
    //slave address with write bit    10010010
    I2C1TRN = 0b01010010; // 8-bits consisting of the slave address (0x29 = 0b0101001) with the write bit (0)
    while(I2C1STATbits.TRSTAT == 1); 
   
    //command byte with register address
    I2C1TRN = 0b10010010; // sending register address = 0x12 for
    while(I2C1STATbits.TRSTAT == 1);
   
    //repeated start condition
    I2C1CONbits.RSEN = 1;   //repeated start
    while(I2C1CONbits.RSEN == 1);   //repeated start
   
    //slave address with read bit
    I2C1TRN = 0b01010011; // 8-bits consisting of the register address (0x29 = 0b0101001) with the read bit (1)
    while(I2C1STATbits.TRSTAT == 1);
   
    //receive data
    I2C1CONbits.RCEN = 1;
    while(I2C1STATbits.RBF == 0);
    device_id = I2C1RCV;
   
    //sending NACK signal from pic    
    I2C1CONbits.ACKDT = 1;              
    I2C1CONbits.ACKEN = 1;              
    while(I2C1CONbits.ACKEN == 1);      
   
    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN == 1);
    //Wait for PEN==0 // PEN will clear when Stop bit is complete
   
    return device_id; //should be 0x44 or 68 in decimal
}

void color_init(){    //looked like it should have a little more
    I2C1BRG = 157;          //100 kHz Baud Rate
    IFS1bits.MI2C1IF = 0;       //SDA1 flag is under IFS1 
    I2C1CONbits.I2CEN = 1;      
   
    delay_ms(40);
    
    //writing RGBC timing register: ATIME
    i2c_write_byte(0x29, 0x01, 0x00);       //writing 0 for maximum sensitivity
    
    //writing control register: CONTROL
    i2c_write_byte(0x29, 0x0F, 0x00);       //writing 0 for a gain of 1
    
    //writing enable register: ENABLE
    i2c_write_byte(0x29, 0x01, 0x03);       //ADC and internal oscillator on

    delay_ms(1);
}

void initBuffer(){
    int i;
    for(i = 0; i < BUFSIZE; i++){
        clear_buffer[BUFSIZE] = 0;
        red_buffer[BUFSIZE] = 0;
        green_buffer[BUFSIZE] = 0;
        blue_buffer[BUFSIZE] = 0;
    }
}

void putClear(int clear){
    clear_buffer[buffer_index++] = clear;
    if(buffer_index >= BUFSIZE){
        buffer_index = 0;
    }
}

void putRed(int red){
    red_buffer[buffer_index++] = red;
    if(buffer_index >= BUFSIZE){
        buffer_index = 0;
    }
}

void putGreen(int green){
    green_buffer[buffer_index++] = green;
    if(buffer_index >= BUFSIZE){
        buffer_index = 0;
    }
}

void putBlue(int blue){
    blue_buffer[buffer_index++] = blue;
    if(buffer_index >= BUFSIZE){
        buffer_index = 0;
    }
}

unsigned int clearAvg(){
    int i;
    unsigned long int clear_avg = 0;
    for(i = 0; i < NUMSAMPLES; i++){
        clear_avg += clear_buffer[i];
    }
    clear_avg = clear_avg/NUMSAMPLES;
    return clear_avg;
}

unsigned int redAvg(){
    int i;
    unsigned long int red_avg = 0;
    for(i = 0; i < NUMSAMPLES; i++){
        red_avg += red_buffer[i];
    }
    red_avg = red_avg/NUMSAMPLES;
    return red_avg;
}

unsigned int greenAvg(){
    int i;
    unsigned long int green_avg = 0;
    for(i = 0; i < NUMSAMPLES; i++){
        green_avg += green_buffer[i];
    }
    green_avg = green_avg/NUMSAMPLES;
    return green_avg;
}

unsigned int blueAvg(){
    int i;
    unsigned long int blue_avg = 0;
    for(i = 0; i < NUMSAMPLES; i++){
        blue_avg += blue_buffer[i];
    }
    blue_avg = blue_avg/NUMSAMPLES;
    return blue_avg;
}

void initializeButton(){
    TRISBbits.TRISB15 = 1; // Set the button pin as input
    CNPU1bits.CN11PUE = 1; // Enable the internal pull-up resistor
}
     
void get_color_avg(void){
    int x = 5;
    while(x-- >= 0){
        unsigned int clear = 0, red = 0, green = 0, blue = 0;
             
        //16 bit clear value
        read_color_clear();
        clear += data_clear[1];
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
    
    clear_avg = clearAvg();
    red_avg = redAvg();
    green_avg = greenAvg();
    blue_avg = blueAvg();
} 

char identify_color(void){ 
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

void masterCodeGen(void){
    int colors[8] = {0,1,2,3};
    
    for(int masterIndex = 0; masterIndex < 4; masterIndex++){        
        
        int randomIndex = rand() % 4 + 0;
        masterCode[masterIndex] = colors[randomIndex];
    }
}

void get_color_inputs(void){
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

void calculateCompare(void){
    //(2) = x - completely wrong
    //(0) = 0 - there is a color in the code, wrong spot
    //(1) = 1 - correct color in the right spot
    int codePos;
    didYouWin = 0;
    //Color matches current place
    for(codePos = 0; codePos < 4; codePos++){
        int x = 0;
        if(userCode[codePos] == masterCode[codePos]){       //Correct color and spot
            codeCompare[codePos] = 1;
            didYouWin++;
        }       
        if(userCode[codePos] != masterCode[codePos]){       //Incorrect color but could be in a different spot          
            for(int i = 0; i < 4; i++){
                if(userCode[codePos] == masterCode[i]){     
                    codeCompare[codePos] = 0;               //color in different spot
                    x = 1;
                }
                else if((userCode[codePos] != masterCode[i]) && (x == 0)){
                    codeCompare[codePos] = 2;               //color not in sequence
                }
            }
        }
    }
}

void printCompare(int row){
    lcd_setCursor(row,0);
    for(int pos = 0; pos < 4; pos++){    
        if(codeCompare[pos] == 0){  //there is a color in the code, wrong spot
            lcd_printChar('0');
        }
        if(codeCompare[pos] == 1){  //correct color in the right spot
            lcd_printChar('1');
        }
        if(codeCompare[pos] == 2){  //completely wrong
            lcd_printChar('X');
        }
    }
}

void winCondition(void){   
    //If the master and user codes are the same the user wins the game
    if(didYouWin == 4){  
        clearLCD();
        while(1){
        lcd_setCursor(0,0);
        lcd_printStr("YOU");
        lcd_setCursor(1,0);
        lcd_printStr("WIN!!!");
        }
    }
}

int main(void) {
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
    printCompare(0);         //print to LCD the progress on row 1
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