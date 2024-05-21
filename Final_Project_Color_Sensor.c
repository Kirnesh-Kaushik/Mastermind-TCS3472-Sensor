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

//first element = lower byte
//second element = higher byte
unsigned char data_clear[2] = {0,0};// clear data stored from ADC
unsigned char data_red[2] = {0,0};// red data stored from ADC
unsigned char data_green[2] = {0,0};// green data stored from ADC
unsigned char data_blue[2] = {0,0}; // blue data stored from ADC


void read_color_clear(void){
    /*
     * Function Desc.: Reads 2 bytes of the clear value from two data registers inside sensor using I2C combined read format and a command code. 
     * Arguments: N/A
     * Peripherals: flora color sensor
    */
    //SEN = 1; //Initiate Start condition
    I2C1CONbits.SEN = 1;    
    while(I2C1CONbits.SEN == 1);
   
    //slave address with write bit    10010010
    I2C1TRN = 0b01010010; // 8-bits consisting of the slave address (0x29 = 0b0101001) with the write bit (0)
    while(I2C1STATbits.TRSTAT == 1); 
   
    //command byte with register address
    I2C1TRN = 0b10110100; //command code: sending register address = 0x14 with auto increment
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
    /*
     * Function Desc.: Reads 2 bytes of the red value from two data registers inside sensor using I2C combined read format and a command code. 
     * Arguments: N/A
     * Peripherals: flora color sensor
    */
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
    /*
     * Function Desc.: Reads 2 bytes of the green value from two data registers inside sensor using I2C combined read format and a command code. 
     * Arguments: N/A
     * Peripherals: flora color sensor
    */
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
    /*
     * Function Desc.: Reads 2 bytes of the blue value from two data registers inside sensor using I2C combined read format and a command code. 
     * Arguments: N/A
     * Peripherals: flora color sensor
    */
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
    /*
     * Function Desc.: Writes a byte to the flora color sensor using an I2C write protocol.
     * Arguments: N/A
     * Peripherals: flora color sensor
    */
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
    /*
     * Function Desc.: Reads the 1 byte device id from the id register inside sensor using I2C combined read format and a command code. 
     * Arguments: N/A
     * Peripherals: flora color sensor
    */
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

void color_init(){
    /*
     * Function Desc.: Initializes the flora color sensor to turn on read mode, have maximum sensitivity, and a gain of 1.
     * Arguments: N/A
     * Peripherals: flora color sensor
    */
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